/*
    $Id: mintaka.c 1776 2007-12-17 11:05:01Z drodenas $
    
	Mintaka ParaverTrace Generator Library
	Copyright (C) 2007 - David Rodenas Pico <david.rodenas@gmail.es>

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

#include <stdlib.h>

/* Mintaka library state **********************************************/

#ifndef DISABLE_TLS
__thread mintaka_thread_info mintaka__thread_info;
#endif /* ifdef DISABLE_TLS */
#ifdef ENABLE_TP
mintaka_thread_info mintaka__thread_info;
#pragma omp threadprivate(mintaka__thread_info)
#endif

int mintaka__enable= 0;

/* Stuff */
mintaka_thread_info* mintaka_get_thread_info(void)
{
	return &mintaka__thread_info;
}

/* All processor initialization and finalization. ***********************/


/** 
 * Initializates a thread. 
 */
void 
mintaka_thread_begin(int processor, int thread)
{
	char file_name[4096];
	int proc_num= processor;
	int th_num= thread - 1;

	mintaka_thread_info* info= mintaka_get_thread_info();
	
	sprintf(file_name,MINTAKA_FILE_FORMAT,mintaka_get_filebase(),proc_num,th_num);
	info->file= fopen(file_name,"w");
	info->current= 0;
	info->processor= proc_num;
	info->thread= th_num;
	info->record[MINTAKA_BUFFER_SIZE-1].state= -1;
	
	mintaka_record1(MINTAKA_STATE_IDLE);

	if (!mintaka__enable) return;
	
}

/** 
 * Finalizates a thread. 
 */
void 
mintaka_thread_end()
{
	mintaka_set_state(MINTAKA_STATE_STOP);

	mintaka_thread_info* info= mintaka_get_thread_info();

	if (info->current)
	fwrite(
		&info->record[0],
		sizeof(struct mintaka_record),
		info->current,
		info->file);
	fclose(info->file);
	
	info->current= 0;
	info->processor= 1;
	info->thread= 1;
	info->file= NULL;

	if (!mintaka__enable) return;
	
}


/* Internal helpers ***************************************************/ 
 

/** 
 * Internal: record creation 
 */
void 
mintaka_flush_buffer()
{
	mintaka_event(MINTAKA_EVENT_FLUSH, MINTAKA_VALUE_BEGIN);

	mintaka_thread_info* info= mintaka_get_thread_info();
	
	assert(info->current == MINTAKA_BUFFER_SIZE);
	fwrite(
		info->record,
		sizeof(struct mintaka_record),
		MINTAKA_BUFFER_SIZE,
		info->file);
	info->current= 0;

	mintaka_event(MINTAKA_EVENT_FLUSH, MINTAKA_VALUE_END);
}




/* Initialization and finalization ************************************/


void 
mintaka_app_begin()
{
	char* mintaka_enable= getenv("MINTAKA_ENABLE");
	if (mintaka_enable)
	{	
		mintaka__enable= atoi(mintaka_enable);
	}
	else
	{
		mintaka__enable= 1;
	}
}

void
mintaka_app_end()
{ 
	mintaka__enable= 0;
}


