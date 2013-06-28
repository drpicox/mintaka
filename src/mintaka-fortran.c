/*
    $Id: mintaka-fortran.c 1591 2007-06-28 14:41:19Z drodenas $
    
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
*/
#include "mintaka.h"


void 
mintaka_app_begin_()
{
	mintaka_app_begin();
}
void 
mintaka_app_end_()
{
	mintaka_app_end();
}
void 
mintaka_merge_()
{
	mintaka_merge();
}

/* Thread initialization and finalization. Begin to use. */
void 
mintaka_thread_begin_(int* processor, int* thread)
{
	mintaka_thread_begin(*processor, *thread);
}
void 
mintaka_thread_end_()
{
	mintaka_thread_end();
}

/* States */
void 
mintaka_state_run_()
{
	mintaka_state_run();
}
void 
mintaka_state_schedule_()
{
	mintaka_state_schedule();
}
void 
mintaka_state_block_()
{
	mintaka_state_block();
}
void 
mintaka_state_synch_()
{
	mintaka_state_synch();
}
void 
mintaka_state_idle_()
{
	mintaka_state_idle();
}
void 
mintaka_set_state_(int* state)
{
	mintaka_set_state(*state);
}
int  
mintaka_get_state_()
{
	return mintaka_get_state_();
}

/* Event */
void 
mintaka_event_(int* event, uint64_t* value)
{
	mintaka_event(*event, *value);
}

/* Event indexing. */
void 
mintaka_index_event_(int *event, const char* description)
{
	mintaka_index_event(*event, description);
}
int  
mintaka_index_get_(const char* string, int *number)
{
	return mintaka_index_get(string, *number);
}
int  
mintaka_index_allocate_(const char* string, int* number, int* event)
{
	return mintaka_index_allocate(string,*number,*event);
}
int  
mintaka_index_allocate2_(const char* string, int*number, const char* string2, int*event)
{
	return mintaka_index_allocate2(string,*number,string2,*event);
}
int  
mintaka_index_allocate3_(const char* string, int*number, const char* string2, const char* string3, int*event)
{
	return mintaka_index_allocate3(string,*number,string2,string3,*event);
}
void mintaka_index_generate_()
{
	mintaka_index_generate();
}


/* Thread messaging support. */
void 
mintaka_wait_send_(uint32_t* channel, int* size)
{
	mintaka_wait_send(*channel, *size);
}
void 
mintaka_send_(uint32_t* channel, int* size)
{
	mintaka_send(*channel, *size);
}
void 
mintaka_wait_receive_(uint32_t* channel, int* size)
{
	mintaka_wait_receive(*channel, *size);
}
void 
mintaka_receive_(uint32_t* channel, int* size)
{
	mintaka_receive(*channel, *size);
}
