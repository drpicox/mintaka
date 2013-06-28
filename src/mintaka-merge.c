/*
    $Id: mintaka-merge.c 1597 2007-06-29 10:40:56Z drodenas $
    
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
#include <stdio.h>



#include "mintaka.h"

#ifdef DISABLE_TLS
/* Default TLS support ************************************************/
static mintaka_thread_info* 
mintaka_get_thread_info(void)
{
	static mintaka_thread_info info;
	return &info;
}
#endif /* ifdef DISABLE_TLS */
/* Custom TS support **************************************************/
#ifdef MINTAKA_CUSTOM_TS
static uint64_t 
mintaka_get_ts(void) 
{
	uint64_t ts= 0;
	return ts++; 
}
#endif


int
main(int argc, char** argv) 
{
	if (argc != 2) {
		printf("usange:\n"
		"\n"
		"%s <tracefilebase>\n", argv[0]);
		
		return 1;
	}
	
	char* filename_base= argv[1];
	
	mintaka_set_filebase(filename_base);
	mintaka_app_begin();
	mintaka_merge();
	mintaka_app_end();
	
	return 0;
}
