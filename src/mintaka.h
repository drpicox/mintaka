/*
    $Id: mintaka.h 1776 2007-12-17 11:05:01Z drodenas $
    
	Mintaka ParaverTrace Generator Library
	Copyright (C) 2007 - David Rodenas Pico <david.rodenas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
    
    Important defines:
    DISABLE_TLS for non TLS machines
    	need to define: 
    		typedef struct mintaka_thread_info mintaka_thread_info;
    		static mintaka_thread_info*(void) { ... }
    ENABLE_TP for non TLS but #pragma openmp threadprivate directive
    MINTAKA_CUSTOM_TS: for custom definition of timestamps
		need to define:
			static inline uint64_t mintaka_get_ts(void) { ... }
		
*/
#ifndef MINTAKA_H_
#define MINTAKA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Thread state ******************************************** info     */
typedef struct mintaka_thread_info mintaka_thread_info;
mintaka_thread_info* mintaka_get_thread_info(void);

/* Base file name ****************************************** basename */
void mintaka_set_filebase(const char* tracefile_basename);
void mintaka_set_fullpath_filebase(const char* tracefile_basename);
const char* mintaka_get_filebase(void);

/* All process initialization and finalization. ************ app      */
void mintaka_app_begin(void);
void mintaka_app_end(void);
void mintaka_merge(void);
static void mintaka_set_enable(int enable); // default getenv("MINTAKA_ENABLE");
static int  mintaka_get_enable(void);

/* Thread initialization and finalization. ***************** thread   */
void mintaka_thread_begin(int processor, int thread);
void mintaka_thread_end(void);

/* States ************************************************** state    */
static void mintaka_state_run(void);
static void mintaka_state_schedule(void);
static void mintaka_state_block(void);
static void mintaka_state_synch(void);
static void mintaka_state_idle(void);
static void mintaka_set_state(int state);
static int  mintaka_get_state(void);

/* Event *************************************************** event    */
static void mintaka_event(int event, uint64_t value);

/* State and event ***************************************** mix      */
static void mintaka_state_and_event(int state, int event, uint64_t value);

/* Event indexing. ***************************************** index    */
void mintaka_index_event(int event, const char* description);
int  mintaka_index_get(const char* string, int number);
static int  mintaka_index_allocate(const char* string, int number, int event);
static int  mintaka_index_allocate2(const char* string, int number, const char* string2, int event);
int  mintaka_index_allocate3(const char* string, int number, const char* string2, const char* string3, int event);
void mintaka_index_generate(void);

/* Thread messaging support. ******************************* messg    */
typedef intptr_t channel_t;
static void mintaka_wait_send(channel_t channel, int size);
static void mintaka_send(channel_t channel, int size);
static void mintaka_wait_receive(channel_t channel, int size);
static void mintaka_receive(channel_t channel, int size);

/* Multiple clock synchronization. ************************* clock    */
static void mintaka_clock_synchronize(uint64_t ts);

/* Event buffer manipulation ******************************* buffer   */
static void mintaka_ensure_buffer(int event_count);
static void mintaka_send_and_state(channel_t channel, int size, int state);
static void mintaka_receive_and_state(channel_t channel, int size, int state);

/* Internal: Thread event support. - - - - - - - - - - - - - - - - - -*/
static void mintaka_record1(int event);
static void mintaka_record2(int event, uint64_t value);
static void mintaka_record3(int event, channel_t value, uint32_t size);
static void mintaka_record1_at(int event, uint64_t ts);
static void mintaka_record2_at(int event, uint64_t value, uint64_t ts);
static void mintaka_record3_at(int event, channel_t lvalue, uint32_t hvalue, uint64_t time);

/* Internal: Gets the current time in tseconds - - - - - - - - - -*/
static uint64_t mintaka_get_ts(void);
/* Internal: record creation - - - - - - - - - - - - - - - - - - - - -*/
void mintaka_flush_buffer(void);



/* ********************************************************************
 * Internal details.
 * 
 * 
 * 
 * 
 * 
 */

#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

/* Constants **********************************************************/

#define MINTAKA_BUFFER_SIZE 4096
#define MINTAKA_FILE_FORMAT "%s.%d.%d"

enum mintaka_state { 
	MINTAKA_STATE_STOP=           14, /* paraver tracing disabled */
	MINTAKA_STATE_RUN=             1, /* paraver tracing run */
	MINTAKA_STATE_SCHEDULE=        7, /* paraver library code */
    MINTAKA_STATE_IDLE=            0, /* paraver idle */
    MINTAKA_STATE_BLOCK=           4, /* paraver thread block */
    MINTAKA_STATE_SYNCH=           5, /* paraver thread block */
    MINTAKA_STATE_WAIT_SEND=       9, /* paraver block sending */
    MINTAKA_STATE_SEND=           10, /* paraver sendind non block */
    MINTAKA_STATE_WAIT_RECEIVE=    3, /* paraver wait message */
    MINTAKA_STATE_RECEIVE=        11, /* paraver wait message non block */
    MINTAKA_STATE_COUNT=          16
};

#define MINTAKA_EVENT_FLUSH    40000003 /* trace flush event */
#define MINTAKA_VALUE_BEGIN           1 /* begin an event */
#define MINTAKA_VALUE_END             0 /* finalizes the event */


/* Record structure ***************************************************/

struct mintaka_record {
	uint32_t  state; /* or event */
	uint16_t  processor;
	uint16_t  thread;
	uint64_t  time;
	channel_t channel; /* or event value */
	uint32_t  size; /* or event value high */
};


/* Internal state *****************************************************/

struct mintaka_thread_info
{
	int                   processor;
	int                   thread;
	int                   state;
	struct mintaka_record record[MINTAKA_BUFFER_SIZE];
	int                   current; /* current record number from buffer. */
	FILE*                 file;
	uint64_t              ts_offset;
};
extern int                          mintaka__enable;

#if defined(ENABLE_TP) && !defined(DISABLE_TLS)
#define DISABLE_TLS
#endif
#ifndef DISABLE_TLS
/* Default TLS support ************************************************/
extern __thread mintaka_thread_info mintaka__thread_info;
#endif /* ifdef DISABLE_TLS */

/* Internal helper functions ******************************************/

#ifndef MINTAKA_CUSTOM_TS
/**
 * Internal: Gets the current time in tseconds.
 * 
 * Should be translated to another call without system call.
 */
static inline uint64_t 
mintaka_get_ts(void)
{
	struct timeval  tp;
	struct timezone tzp;
	
	uint64_t ts_offset= mintaka_get_thread_info()->ts_offset;

	(&gettimeofday)(&tp, &tzp);
	return (tp.tv_sec*(uint64_t)1000000+tp.tv_usec) - ts_offset;
}
#endif

/** 
 * Internal: record creation 
 */
static inline void 
mintaka_record3_at(int state, channel_t channel, uint32_t size, uint64_t time)
{
	if (!mintaka__enable) return;
	
	int i= mintaka_get_thread_info()->current;
	int processor= mintaka_get_thread_info()->processor;
	int thread= mintaka_get_thread_info()->thread;
	struct mintaka_record* record= &mintaka_get_thread_info()->record[0];	
	
	record[i].state= state;
	record[i].processor= processor;
	record[i].thread= thread;
	record[i].time= time;
	record[i].channel= channel;
	record[i].size= size;
	mintaka_get_thread_info()->current= i + 1;
	if (i == MINTAKA_BUFFER_SIZE - 2)
	{
		mintaka_flush_buffer();
	}
}

/** 
 * Internal: record creation 
 */
static inline void 
mintaka_record3(int state, channel_t channel, uint32_t size)
{
	uint64_t ts= mintaka_get_ts();
	
	mintaka_record3_at(state, channel, size, ts);
}

/** 
 * Internal: record creation 
 */
static inline void 
mintaka_record1(int state)
{
	mintaka_record3(state, 0, 0);
}

/** 
 * Internal: record creation 
 */
static inline void 
mintaka_record1_at(int state, uint64_t ts)
{
	mintaka_record3_at(state, 0, 0, ts);
}

/** 
 * Internal: record creation 
 */
static inline void 
mintaka_record2(int event, uint64_t value)
{
	uint32_t low= (uint32_t)value;
	uint32_t high= (uint32_t)(value >> 32);
	mintaka_record3(event, low, high);
}

/** 
 * Internal: record creation 
 */
static inline void 
mintaka_record2_at(int event, uint64_t value, uint64_t ts)
{
	uint32_t low= (uint32_t)value;
	uint32_t high= (uint32_t)(value >> 32);
	mintaka_record3_at(event, low, high, ts);
}

/**
 * Ensure a minimum capacity at flush buffer.
 */
static inline void 
mintaka_ensure_buffer(int event_count)
{
	assert(event_count >= 0);
	assert(event_count < MINTAKA_BUFFER_SIZE - 2);
	
	if (mintaka_get_thread_info()->current 
		> MINTAKA_BUFFER_SIZE - 2 - event_count)
	{
		mintaka_flush_buffer();
	}
}


/* Enable functions ***************************************************/
static inline void
mintaka_set_enable(int enable)
{
	mintaka__enable= enable;
}

static inline int
mintaka_get_enable(void)
{
	return mintaka__enable;
}

/* State generation ***************************************************/

/** 
 * Switches to requestet state.
 * Thread is doing nothing.
 */
static inline void 
mintaka_set_state(int state)
{
	assert(state < MINTAKA_STATE_COUNT);
	
	mintaka_get_thread_info()->state= state;
	mintaka_record1(state);
}

/** 
 * Switches to requestet state.
 * Thread is doing nothing.
 */
static inline void 
mintaka_set_state_at(int state, uint64_t ts)
{
	assert(state < MINTAKA_STATE_COUNT);
	
	mintaka_get_thread_info()->state= state;
	mintaka_record1_at(state, ts);
}

/**
 * Query about current state.
 */
static inline int
mintaka_get_state(void)
{
	return mintaka_get_thread_info()->state;
}

/** 
 * Switches to run state.
 * All threads start at initalization state (schedulling), you need explicity
 * to change run.
 */
static inline void 
mintaka_state_run(void)
{
	mintaka_set_state(MINTAKA_STATE_RUN);
}

/** 
 * Switches to schedule, or library, state.
 */
static inline void 
mintaka_state_schedule(void)
{
	mintaka_set_state(MINTAKA_STATE_SCHEDULE);
}

/** 
 * Switches to block state.
 * Lost processor.
 */
static inline void 
mintaka_state_block(void)
{
	mintaka_set_state(MINTAKA_STATE_BLOCK);
}

/** 
 * Switches to synchronization state.
 * Lock wait, barrier, ...
 */
static inline void 
mintaka_state_synch(void)
{
	mintaka_set_state(MINTAKA_STATE_SYNCH);
}

/** 
 * Switches to idle state.
 * Thread is doing nothing.
 */
static inline void 
mintaka_state_idle(void)
{
	mintaka_set_state(MINTAKA_STATE_IDLE);
}





/* Event generation ***************************************************/

/** 
 * Generates a new event with selected value.
 * Event value MUST not conflict with STATE VALUES.
 */ 
static inline void 
mintaka_event(int event, uint64_t value)
{
	assert(event >= MINTAKA_STATE_COUNT || event < 0);
	
	mintaka_record2(event, value);
}

/** 
 * Generates a new event with selected value.
 * Event value MUST not conflict with STATE VALUES.
 */ 
static inline void 
mintaka_event_at(int event, uint64_t value, uint64_t ts)
{
	assert(event >= MINTAKA_STATE_COUNT || event < 0);
	
	mintaka_record2_at(event, value, ts);
}

/* Messages registry **************************************************/

/** 
 * Record a wait to send a data through a channel because channel is busy 
 * or not aviable at moment. 
 * It also changes the sate. 
 * For each message thera are at least one wait_send, and size is the same
 * that send.
 */
static inline void 
mintaka_wait_send(channel_t channel, int size)
{
	int current= mintaka_get_thread_info()->current;
	struct mintaka_record* record= &mintaka_get_thread_info()->record[0];
	
	assert(current > 0 || !mintaka__enable);
	
	// do not write if the previous one was a repeated wait
	int i= (current - 1 + MINTAKA_BUFFER_SIZE) % MINTAKA_BUFFER_SIZE;  
	if (record[i].state == MINTAKA_STATE_WAIT_SEND &&
		record[i].channel == (uint32_t)channel &&
		record[i].size == size)
	{} else
	mintaka_record3(MINTAKA_STATE_WAIT_SEND, channel, size);
}

/** 
 * Record the data send through a channel. 
 * It also changes the sate. 
 * For each message thera are one and only one send, and size is the same
 * that received.
 */
static inline void 
mintaka_send(channel_t channel, int size)
{
	mintaka_record3(MINTAKA_STATE_SEND, channel, size);
}
static inline void 
mintaka_send_at(channel_t channel, int size, uint64_t ts)
{
	mintaka_record3_at(MINTAKA_STATE_SEND, channel, size, ts);
}

/**
 * Record a wait to receive data, because is not aviable o because the
 * channel is not ready.  
 * For each message there are at least one wait_receive, and size is the same
 * that receive.
 */
static inline void 
mintaka_wait_receive(channel_t channel, int size)
{
	int current= mintaka_get_thread_info()->current;
	struct mintaka_record* record= &mintaka_get_thread_info()->record[0];

	assert(current > 0 || !mintaka__enable);
	
	// do not write if the previous one was a repeated wait
	int i= (current - 1 + MINTAKA_BUFFER_SIZE) % MINTAKA_BUFFER_SIZE; 
	if (record[i].state == MINTAKA_STATE_WAIT_RECEIVE &&
		record[i].channel == (uint32_t)channel &&
		record[i].size == size)
	{} else
	mintaka_record3(MINTAKA_STATE_WAIT_RECEIVE, channel, size);
}

/**
 * Record the data reception from a channel.
 * It also changes the sate. 
 * For each message thera are one and only one send, and size is the same
 * that send.
 */
static inline void 
mintaka_receive(channel_t channel, int size)
{
	mintaka_record3(MINTAKA_STATE_RECEIVE, channel, size);
}
static inline void 
mintaka_receive_at(channel_t channel, int size, uint64_t ts)
{
	mintaka_record3_at(MINTAKA_STATE_RECEIVE, channel, size, ts);
}

/* Event and state simultaneous generation ****************************/

static inline void 
mintaka_state_and_event(int state, int event, uint64_t value)
{
	if (!mintaka__enable) return;
	assert(event >= MINTAKA_STATE_COUNT || event < 0);	

	mintaka_ensure_buffer(2);
	uint64_t ts= mintaka_get_ts();	
	mintaka_set_state_at(state, ts);
	mintaka_event_at(event, value, ts);
}

static inline void 
mintaka_send_and_state(channel_t channel, int size, int state)
{
	if (!mintaka__enable) return;

	mintaka_ensure_buffer(2);
	uint64_t ts= mintaka_get_ts();	
	mintaka_send_at(channel, size, ts);
	mintaka_set_state_at(state, ts);
}

static inline void 
mintaka_receive_and_state(channel_t channel, int size, int state)
{
	if (!mintaka__enable) return;

	mintaka_ensure_buffer(2);
	uint64_t ts= mintaka_get_ts();	
	mintaka_receive_at(channel, size, ts);
	mintaka_set_state_at(state, ts);
}

/* Event indexing *****************************************************/

/**
 * Allocates one index value with one string argument.
 */
static inline int  
mintaka_index_allocate(const char* string, int number, int event)
{
	return mintaka_index_allocate3(string, number, NULL, NULL, event);
}
/**
 * Allocates one index value with two string argument.
 */
static inline int  
mintaka_index_allocate2(const char* string, int number, const char* string2, int event)
{
	return mintaka_index_allocate3(string, number, string2, NULL, event);
}


/**
 * Synchronize the clocks for multiple machines.
 * It adds one event that will be used as mark by the merger as
 * synchronization point.
 */
static inline void 
mintaka_clock_synchronize(uint64_t ts)
{
	uint64_t current;
	
	current= mintaka_get_ts();
	assert(current < ts);
	
	// ts == current - mintaka__ts_offset;
	mintaka_get_thread_info()->ts_offset= current - ts;
}


#ifdef __cplusplus
}
#endif

#endif /* MINTAKA_H_ */
