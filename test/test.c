/*
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
/*
 * This is a simple test example.
 * 
 * David Rodenas Pico (2007)
 */

#include "../src/mintaka.h"

// allocate is always used because we have no parallelism, is NOT thread safe
#define LINE_EVENT   (5999)
#define LINE_VALUE   (mintaka_index_allocate(__FILE__, __LINE__, LINE_EVENT))

static const char* CHECK_FILE= "check";
#define CHECK_EVENT   (5998)

int
main(int argc, char** argv) 
{
	char* filename_base= "trace";


    // ---- Event generation
	// Generic
	mintaka_set_fullpath_filebase(filename_base);
	mintaka_app_begin();
	
	
	// Thread sending, 1
	mintaka_thread_begin(1, 1);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_state_idle();
	mintaka_state_schedule();
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_state_run();
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_wait_send(1, 32);
	mintaka_send(1, 32);
	mintaka_state_schedule();
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_state_run();
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_send(1, 32);
	mintaka_state_synch();
	mintaka_state_block();
	mintaka_state_schedule();
	mintaka_event(LINE_EVENT, 0);
	mintaka_thread_end();


	// Receiving thread	
	mintaka_thread_begin(1, 2);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_state_idle();
	mintaka_state_schedule();
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_state_run();
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_wait_receive(1, 32);
	mintaka_receive(1, 32);
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_state_run();
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);
	mintaka_receive(1, 32);
	mintaka_state_synch();
	mintaka_state_block();
	mintaka_state_schedule();
	mintaka_event(LINE_EVENT, 0);
	mintaka_thread_end();
	
	
	// Overhead compute thread	
	mintaka_thread_begin(1, 3);
	mintaka_state_idle();
	mintaka_event(LINE_EVENT, LINE_VALUE);
	
	mintaka_state_run();
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_state_schedule();
	
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);

	mintaka_state_run();
	int l1= mintaka_index_allocate(CHECK_FILE, 1, CHECK_EVENT);
	mintaka_state_schedule();
	mintaka_state_run();
	int l2= mintaka_index_allocate(CHECK_FILE, 2, CHECK_EVENT);
	mintaka_state_schedule();
	mintaka_state_run();
	int l3= mintaka_index_allocate(CHECK_FILE, 3, CHECK_EVENT);
	mintaka_state_schedule();
	mintaka_state_run();
	int l4= mintaka_index_allocate(CHECK_FILE, 4, CHECK_EVENT);
	mintaka_state_schedule();
	mintaka_state_run();
	int l5= mintaka_index_allocate(CHECK_FILE, 5, CHECK_EVENT);
	mintaka_state_schedule();
	mintaka_state_run();
	int l6= mintaka_index_allocate(CHECK_FILE, 6, CHECK_EVENT);
	mintaka_state_schedule();

	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);

	mintaka_state_run();
	int d1= mintaka_index_get(CHECK_FILE, 1);
	mintaka_state_schedule();
	mintaka_state_run();
	int d2= mintaka_index_get(CHECK_FILE, 2);
	mintaka_state_schedule();
	mintaka_state_run();
	int d3= mintaka_index_get(CHECK_FILE, 3);
	mintaka_state_schedule();
	mintaka_state_run();
	int d4= mintaka_index_get(CHECK_FILE, 4);
	mintaka_state_schedule();
	mintaka_state_run();
	int d5= mintaka_index_get(CHECK_FILE, 5);
	mintaka_state_schedule();
	mintaka_state_run();
	int d6= mintaka_index_get(CHECK_FILE, 6);
	mintaka_state_schedule();

	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);

	mintaka_state_run();
	mintaka_event(CHECK_EVENT, l1);
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_event(CHECK_EVENT, l2);
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_event(CHECK_EVENT, l3);
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_event(CHECK_EVENT, l4);
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_event(CHECK_EVENT, l5);
	mintaka_state_schedule();
	mintaka_state_run();
	mintaka_event(CHECK_EVENT, l6);
	mintaka_state_schedule();
	mintaka_state_idle();
	
	mintaka_event(LINE_EVENT, 0);
	mintaka_event(LINE_EVENT, LINE_VALUE);

	assert(l1 == d1);
	assert(l2 == d2);
	assert(l3 == d3);
	assert(l4 == d4);
	assert(l5 == d5);
	assert(l6 == d6);
	
	mintaka_event(CHECK_EVENT, 0);
	mintaka_event(CHECK_EVENT, 0);
	mintaka_event(CHECK_EVENT, 0);
	mintaka_event(CHECK_EVENT, 0);
	mintaka_event(CHECK_EVENT, 0);
	mintaka_event(CHECK_EVENT, 0);
	mintaka_event(LINE_EVENT, 0);
	mintaka_thread_end();
	
	
	// Generic
	mintaka_app_end();
	mintaka_merge();
	
	

	// ---- PCF generation and check (complete independent)
	mintaka_index_event(5998, "CheckEvent");
	mintaka_index_event(5999, "SampleEvent");
	mintaka_index_event(6000, "SomeKindEvent");
	mintaka_index_event(6001, "SomeOtherKindEvent");
	mintaka_index_event(6002, "SomeNoUseKindEvent");
	
	int f1_1= mintaka_index_get("File1", 1);
	assert(f1_1 == -1);
	f1_1= mintaka_index_allocate("File1", 1, 6000);
	assert(f1_1 == mintaka_index_get("File1", 1));
	
	int f1_2= mintaka_index_get("File1", 2);
	assert(f1_2 == -1);
	f1_2= mintaka_index_allocate("File1", 2, 6001);
	assert(f1_2 == mintaka_index_get("File1", 2));
	
	int o1_51= mintaka_index_get("Other1", 51);
	assert(o1_51 == -1);
	o1_51= mintaka_index_allocate("Other1", 51, 6000);
	assert(o1_51 == mintaka_index_get("Other1", 51));

	assert(f1_1 == mintaka_index_get("File1", 1));
	assert(o1_51 == mintaka_index_get("Other1", 51));
	assert(f1_2 == mintaka_index_get("File1", 2));
	assert(f1_1 != o1_51);
	assert(f1_2 != o1_51);
	assert(f1_1 != f1_2);
	
	mintaka_index_generate();
	




	return 0;
}
