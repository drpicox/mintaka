/*
    $Id: mintaka-merger.c 1775 2007-12-17 10:42:12Z drodenas $
    
	Mintaka ParaverTrace Generator Library
	Copyright (C) 2007 - David Rodenas Pico <david.rodenas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "mintaka.h"



/* ********************************************************************
 * 
 * 
 *  MINTAKA 2 PRV!!
 * 
 * 
 * ********************************************************************
 */


#include "mintaka.h"


#include <assert.h>


#define PROCESSOR_COUNT     128


static struct {
	FILE* file;
	int processor;
	int thread;
} _processors[PROCESSOR_COUNT];
static int _processor_count;

static struct {
	int consumed;
	struct mintaka_record record;
} _peeks[PROCESSOR_COUNT];

static struct mintaka_record*
record_peek1(int processor)
{
	struct mintaka_record* peek;
	int p;
	
	p= processor;
	if (_peeks[p].consumed)
	{
		FILE* file= _processors[p].file;
		fread(&_peeks[p].record, sizeof(struct mintaka_record), 1, file);
		if (!feof(file)) 
		{
			_peeks[p].consumed= 0;
			peek= &_peeks[p].record;
			
			assert(peek->processor == _processors[p].processor); 
			assert(peek->thread == _processors[p].thread); 
		} else {
			peek= NULL;
		}
	} else {
		peek= &_peeks[p].record;
	}

	return peek;	
}

static struct mintaka_record*
record_peek(int* processor)
{
	int p;
	struct mintaka_record* next= NULL;
	
	for (p= 0; p < _processor_count; p++)
	{
		struct mintaka_record* current= record_peek1(p);
		if (next == NULL || 
			(current != NULL && current->time < next->time))
		{
			next= current;
			*processor= p;
		} 
	}
	
	return next;
}

static struct mintaka_record*
record_next()
{
	int p= 0;
	struct mintaka_record* next;

	next= record_peek(&p);
	if (next != NULL)
	{
		_peeks[p].consumed= 1;
	}
	
	return next;
}

static void 
record_peeks_init()
{
	int p;
	
	for (p= 0; p < _processor_count; p++)
	{
		_peeks[p].consumed= 1;
		record_peek1(p);
	}
}



static void
join_files()
{
	struct mintaka_record* next;
	FILE* bin_file;

        uint64_t init_time;
	uint64_t fini_time;
	
	char bin_filename[4096];
	sprintf(bin_filename, "%s.bin", mintaka_get_filebase());
	
	bin_file= fopen(bin_filename,"w");
	assert(bin_file);
	
	record_peeks_init();
	next= record_next();
	if (next != NULL) init_time= next->time; else init_time= 0;
	fini_time= init_time;  
	
	while (next != NULL)
	{
		next->time= next->time - init_time;
		fini_time= next->time;
		fwrite(next, sizeof(struct mintaka_record), 1, bin_file);
		next= record_next();
	}	
	
	fclose(bin_file);
}

static int 
open_thread_file(int processor, int thread)
{
	FILE* file;
	char file_name[4096];
	
	sprintf(file_name, MINTAKA_FILE_FORMAT, mintaka_get_filebase(), processor, thread);
	file= fopen(file_name, "r");
	
	if (file != NULL)
	{
		int p= _processor_count;
		_processors[p].file=     file;
		_processors[p].processor= processor;
		_processors[p].thread=     thread;
		_processor_count= p + 1;
	}
	
	return file != NULL;
}

static int
open_thread_files(int processor)
{
	int any_thread;
	int has_file;
	int thread;
	
	any_thread= 0;
	thread= 0;
	do 
	{
		has_file= open_thread_file(processor, thread);
		
		thread= thread + 1;
		any_thread= any_thread || has_file; 
	} while (has_file);
	
	return any_thread;
}

static int
open_processor_files()
{
	int any_processor;
	int any_thread;
	int processor;
	
	any_processor= 0;
	processor= 1;
	do
	{
		any_thread= open_thread_files(processor);
		
		any_processor= any_processor || any_thread;
		processor= processor + 1;
	} while (any_thread);
	
	return any_processor;
} 

static void 
open_files()
{
	_processor_count= 0;
	//int any_processor= 
	open_processor_files();
	//assert(any_processor);
}

static void
close_files()
{
	int p;
	
	for (p= 0; p < _processor_count; p++)
	{
		fclose(_processors[p].file);
	}
	_processor_count= 0;
}



#define STREAM_COUNT       1024 

#define PARAVER_STATE                    1
#define PARAVER_EVENT                    2
#define PARAVER_COMMUNICATION            3

#define PARAVER_STATE_NOT_CREATED        2
#define PARAVER_STATE_STOP              14
#define PARAVER_STATE_INITIALIZE         7
#define PARAVER_STATE_RUN                1
#define PARAVER_STATE_FINALIZE           7
#define PARAVER_STATE_SCHEDULE           7
#define PARAVER_STATE_WAIT_SEND          9
#define PARAVER_STATE_SEND              10
#define PARAVER_STATE_WAIT_RECEIVE           3
#define PARAVER_STATE_RECEIVE               11
#define PARAVER_EVENT_ITERATION      80000


#ifdef DEBUG_MINTAKA
#define PRV_PRINTF(fmt, args...) \
	fprintf(_prv_file, "##%s:%d %s "fmt"\n" , __FILE__, __LINE__, __FUNCTION__, ## args);
#else
#define PRV_PRINTF(fmt, args...) do {} while (0)
#endif


static struct {
	channel_t channel;
	int output_processor;
	int output_thread;
	int input_processor;
	int input_thread;
	
	long output_element;
	long input_element;
	
	long data_sent;
	long data_received;
	
	int size;
	
} _channels[STREAM_COUNT];
static int _channels_count;

static FILE* _prv_file;
static FILE* _bin_file;
static long _bin_current;
static long _bin_size;

static inline int
record_processor(struct mintaka_record* record)
{
	return record->thread + 1;
}

static inline void 
record_seek(long number)
{
	if (number != _bin_current) 
	{
		fseek(_bin_file, number*sizeof(struct mintaka_record), SEEK_SET);		
	}
}

static inline void 
record_read(long number, struct mintaka_record* record)
{
	record_seek(number);
	fread(record, sizeof(struct mintaka_record), 1, _bin_file);
	_bin_current= _bin_current + 1;
}

static inline void
record_find_next(long* current, int p, struct mintaka_record* record)
{
	int found= 0;
	while (*current < _bin_size && !found)
	{
		record_read(*current, record);
		if (record_processor(record) == p) found= 1; else *current= *current + 1;
	}
}

static inline void
record_find_first(int p, struct mintaka_record* record)
{
	long element;
	
	element= 0;
	record_find_next(&element, p, record);
}

static void
prv_header()
{
	struct mintaka_record record;
	int i;
	
	record_read(_bin_size - 1, &record);
	fprintf(_prv_file, "#Paraver (11/11/11 at 11:11):%llu:%d:1:%d(1:1",
		record.time, _processor_count, _processor_count);
	for (i= 1; i < _processor_count; i++)
	{
		fprintf(_prv_file, ",1:%d", i+1);
	}
	fprintf(_prv_file, ")\n");
}

static inline void
prv_state(int p, uint64_t init, uint64_t fini, int state)
{
	fprintf(_prv_file, "1:%d:1:%d:1:%llu:%llu:%d\n",
		p, p, init, fini, state);
}

static void
prv_state_construct(long element, struct mintaka_record* record, int state)
{
	int p;
	struct mintaka_record next;

	p= record_processor(record);
	element= element + 1;
	next.time= record->time;	
	record_find_next(&element, p, &next);
	
	prv_state(p, record->time, next.time, state);
}

static void
prv_event_construct(long element, struct mintaka_record* record, int event)
{
	//struct mintaka_record next;
	uint64_t value;
	int p;
	
	p= record_processor(record);
	value= record->size;
	value= record->channel | (value << 32);
	
	fprintf(_prv_file, "2:%d:1:%d:1:%llu:%d:%llu\n",
		p, p, record->time, event, value);
}

static void
prv_not_created()
{
	int p;
	
	for (p= 1; p <= _processor_count; p++)
	{
		struct mintaka_record record;
		record_find_first(p, &record);
		prv_state(p, 0, record.time, PARAVER_STATE_NOT_CREATED);
	}
}

static int
channel_get(long element, struct mintaka_record* record)
{
	int s;
	
	for (s= 0; s < _channels_count && record->channel != _channels[s].channel; s++);
	
	if (s == _channels_count)
	{
		assert(_channels_count < STREAM_COUNT);
		_channels_count= s + 1;
		_channels[s].channel= record->channel;
		_channels[s].output_processor= 0;
		_channels[s].output_thread= -1;
		_channels[s].input_processor= 0;
		_channels[s].input_thread= -1;
		_channels[s].input_element= element;
		_channels[s].output_element= element;
		_channels[s].data_sent= 0;
		_channels[s].data_received= 0;
	}
	
	
	return s;
}

static void
channel_find_next(long* current, int s, struct mintaka_record* record)
{
	int found= 0;
	while (*current < _bin_size && !found)
	{
		record_read(*current, record);
		if (record->channel == _channels[s].channel) found= 1; else *current= *current + 1;
	}
}

static void
channel_find(long* wait, long* current, int s,
	int wait_state, uint64_t* wait_time, int work_state, uint64_t* work_time,
	struct mintaka_record* record)
{
	int found;
	int wait_found;	
	
	found= 0;
	wait_found= 0;
	while (*current < _bin_size && !found)
	{
		channel_find_next(current, s, record);
		
		if (record->state == wait_state) 
		{
			wait_found= 1;
			*wait= *current;
			*wait_time= record->time;
			*current= *current + 1;
		} else 
		if (record->state == work_state) 
		{
			found= 1;
			*work_time= record->time;
		} else
		{		
			*current= *current + 1;
		} 
	} 
	
	if (!wait_found) 
	{
		*wait= *current;
		*wait_time= *work_time; 
	}
} 

static void 
prv_channel_construct(long current, struct mintaka_record* record)
{
	int s;
	long receive_element= 0, wait_receive_element= 0;
	long send_element= 0, wait_send_element= 0;
	int op, ip, size;
	//int found_wait_receive, found_wait_send, found_receive, found_send;
	uint64_t receive_time, send_time, wait_receive_time, wait_send_time;
	struct mintaka_record send, receive;
	
	receive_time= 0;
	send_time= 0;
	wait_receive_time= 0;
	wait_send_time= 0;
	
	s= channel_get(current, record);
	send_element= _channels[s].input_element;
	receive_element= _channels[s].output_element;

	PRV_PRINTF("channel:%llu s:%u current:%u output:%u input:%u (%d)",
		(uint64_t)record->channel, (uint32_t)s, (uint32_t)current, (uint32_t)send_element, (uint32_t)receive_element
		);

	// Looks for the pair element
	channel_find(&wait_send_element, &send_element, s, MINTAKA_STATE_WAIT_SEND, &wait_send_time, MINTAKA_STATE_SEND, &send_time, &send);
	channel_find(&wait_receive_element, &receive_element, s, MINTAKA_STATE_WAIT_RECEIVE, &wait_receive_time, MINTAKA_STATE_RECEIVE, &receive_time, &receive);		

	if (s)
	PRV_PRINTF("channel:%llu s:%u current:%u woutput:%u winput:%u (%d)",
		(uint64_t)record->channel, (uint32_t)s, (uint32_t)current, (uint32_t)wait_send_element, (uint32_t)wait_receive_element
		);
	
	if (s)
	PRV_PRINTF(
			"wait_send_element:%u wait_receive_element:%u"
			, (unsigned int)wait_send_element, (unsigned int)wait_receive_element
			);
	
	if (s)
	PRV_PRINTF(
			"send_element:%u      receive_element:%u"
			, (unsigned int)send_element, (unsigned int)receive_element
			);
	if (s)
	PRV_PRINTF(
			"bin_size:%u cond:(%d || %d) && %d && %d : %d"
			, (unsigned int)_bin_size
			, current == wait_send_element, current == wait_receive_element
			, send_element < _bin_size, receive_element < _bin_size
			, ((current == wait_send_element || current == wait_receive_element) 
			&& 	send_element < _bin_size && receive_element < _bin_size)
			);
	
	if ((current == wait_send_element || current == wait_receive_element) 
	&& 	send_element < _bin_size && receive_element < _bin_size)
	{	
	op= record_processor(&send);
	ip= record_processor(&receive);
	size= send.size < receive.size ? send.size : receive.size;
	
	fprintf(_prv_file, "3");
	fprintf(_prv_file, ":%u:1:%u:1", op, op);
	fprintf(_prv_file, ":%llu", wait_send_time);
	fprintf(_prv_file, ":%llu", send_time);
	fprintf(_prv_file, ":%u:1:%u:1", ip, ip);
	fprintf(_prv_file, ":%llu", wait_receive_time);
	fprintf(_prv_file, ":%llu", receive_time);
	fprintf(_prv_file, ":%u:%u\n", size, s+1);

	int data_sent= _channels[s].data_sent + send.size;
	int data_received= _channels[s].data_received + receive.size;
	
	PRV_PRINTF("> previous data_sent: %ld, data_received: %ld, current: %ld", _channels[s].data_sent, _channels[s].data_received, current);
	PRV_PRINTF("  input_element: %ld, output_element: %ld", _channels[s].input_element, _channels[s].output_element);
	PRV_PRINTF("  data_sent: %ld, data_received: %ld", data_sent, data_received);	
	
	if (data_sent <= data_received)
	{
		_channels[s].data_sent+= send.size;
		_channels[s].input_element= send_element + 1;
	}
	if (data_sent >= data_received)
	{
		_channels[s].data_received+= receive.size;
		_channels[s].output_element= receive_element + 1;
	}
	PRV_PRINTF("+ final data_sent: %ld, data_received: %ld", _channels[s].data_sent, _channels[s].data_received);
	PRV_PRINTF("  input_element: %ld, output_element: %ld", _channels[s].input_element, _channels[s].output_element);
	if (data_sent > data_received)
	{
		prv_channel_construct(current, record);
	}
	
	}
}

static void
prv_processor_record(long current, struct mintaka_record* record)
{
	int state= record->state;
	
	switch (state)
	{
	case MINTAKA_STATE_WAIT_SEND:
		prv_channel_construct(current, record);
		prv_state_construct(current, record, PARAVER_STATE_WAIT_SEND);
		break;
		
	case MINTAKA_STATE_SEND:
		prv_channel_construct(current, record);
		prv_state_construct(current, record, PARAVER_STATE_SEND);
		break;
		
	case MINTAKA_STATE_WAIT_RECEIVE:
		//prv_channel_construct(current, record);
		prv_state_construct(current, record, PARAVER_STATE_WAIT_RECEIVE);
		break;
		
	case MINTAKA_STATE_RECEIVE:
		//prv_channel_construct(current, record);
		prv_state_construct(current, record, PARAVER_STATE_RECEIVE);
		break;
		
	default:
		if (state >= 0 && state < MINTAKA_STATE_COUNT)
		{
			prv_state_construct(current, record, state);
		}
		else
		{
			prv_event_construct(current, record, state);
		}
		break;

	}
}

static void
prv_processor()
{
	long current;
	for (current= 0; current < _bin_size; current++)
	{
		PRV_PRINTF("%ld", current);
		struct mintaka_record record;
		record_read(current, &record);
		prv_processor_record(current, &record);
	}
}

static void
prv_generate()
{
	char prv_filename[4096];
	char bin_filename[4096];
	sprintf(prv_filename, "%s.prv", mintaka_get_filebase());
	sprintf(bin_filename, "%s.bin", mintaka_get_filebase());
	
	_prv_file= fopen(prv_filename, "w"); assert(_prv_file);
	_bin_file= fopen(bin_filename, "r"); assert(_bin_file);
	fseek(_bin_file, 0, SEEK_END);
	_bin_size= ftell(_bin_file) / sizeof(struct mintaka_record);
	_bin_current= _bin_size;
	
	prv_header();
	prv_not_created();
	prv_processor();
	
	fclose(_bin_file);	
	fclose(_prv_file);	
}




void
mintaka_merge()
{ 
	open_files();
	join_files();
	prv_generate();
	close_files();
}
