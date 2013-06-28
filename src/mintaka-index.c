/*
    $Id: mintaka-index.c 1591 2007-06-28 14:41:19Z drodenas $
    
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

#include <string.h>
#include <stdio.h>



#define MINTAKA__INDEX_EVENTS_SIZE 16
static struct {
	int event;
	const char* description;
} mintaka__index_events[MINTAKA__INDEX_EVENTS_SIZE];
int mintaka__index_events_count;


#define MINTAKA__INDEX_VALUES_SIZE 4093 
static struct {
	const char* string;
	int number;
	const char* string2;
	const char* string3;
	int event;
} mintaka__index_values[MINTAKA__INDEX_VALUES_SIZE]; 
int mintaka__index_values_count;



/* events *************************************************************/
void 
mintaka_index_event(int event, const char* description)
{
	assert(mintaka__index_events_count < MINTAKA__INDEX_EVENTS_SIZE);
	
	int i= mintaka__index_events_count;
	mintaka__index_events[i].event= event;
	mintaka__index_events[i].description= description;
	
	mintaka__index_events_count= mintaka__index_events_count + 1;
}


/* values *************************************************************/
static inline int 
mintaka_index_hash(const char* string, int number)
{
	uint64_t value= 0;
	const char* current= string;
	
	while (*current) 
	{
		value= (value >> 59) ^ (value << 5) ^ *current; 
		current++;
	}
	
	value= value ^ number;
	
	return (int)(value % MINTAKA__INDEX_VALUES_SIZE);
}


static inline int
mintaka_index_find(const char* string, int number)
{
	int index= mintaka_index_hash(string, number);
	
	// if any, and any is diferent line or file
	while (mintaka__index_values[index].string
		&& (mintaka__index_values[index].number != number
		|| (mintaka__index_values[index].string != string
		&& strcmp(mintaka__index_values[index].string, string))))
	{
		index= (index + 1) % MINTAKA__INDEX_VALUES_SIZE;
	}
		
	return index;
}
int 
mintaka_index_get(const char* string, int number)
{
	int index= mintaka_index_find(string, number);
	
	if (mintaka__index_values[index].string == NULL)
	{
		index= -1;
	}
	
	return index;
}
int 
mintaka_index_allocate3(const char* string, int number, 
	const char* string2, const char* string3, int event)
{
	assert(mintaka__index_values_count < MINTAKA__INDEX_VALUES_SIZE - 1);
	
	int index= mintaka_index_find(string, number);

	if (mintaka__index_values[index].string == NULL)
	{
		mintaka__index_values[index].string= string;
		mintaka__index_values[index].number= number;
		mintaka__index_values[index].string2= string2;
		mintaka__index_values[index].string3= string3;
		mintaka__index_values[index].event= event;
		
		mintaka__index_values_count= mintaka__index_values_count + 1;
	}
	
	return index;
}



/* pcf generation *****************************************************/
static void inline
mintaka_index_generate_event(int event, const char* description, FILE* pcf)
{
	int index;
	
	fprintf(pcf, "EVENT_TYPE\n");
	fprintf(pcf, "6 %10d %s\n", event, description);
	fprintf(pcf, "\n");
	fprintf(pcf, "VALUES\n");
	fprintf(pcf,"%d %s\n", 0, "End");

	for (index= 0; index < MINTAKA__INDEX_VALUES_SIZE; index++)
	{
		const char* string= mintaka__index_values[index].string;
		int         number= mintaka__index_values[index].number;
		const char* string2= mintaka__index_values[index].string2;
		const char* string3= mintaka__index_values[index].string3;
		int         index_event= mintaka__index_values[index].event;
		
		if (string != NULL && index_event == event)
		{
			if (string2 == NULL) string2= "";
			if (string3 == NULL) string3= "";
			fprintf(pcf,"%d %s:%d %s %s\n", index, string, number, string2, string3);
		}
	}

	fprintf(pcf,"\n");
}

void
mintaka_index_generate()
{
	char filename[4096];
	int i;
	FILE* pcf;
	
	sprintf(filename,"%s.index.pcf", mintaka_get_filebase());
	
	pcf= fopen(filename, "w"); assert(pcf);

	for (i= 0; i < mintaka__index_events_count; i++)
	{
		int event= mintaka__index_events[i].event;
		const char* description= mintaka__index_events[i].description;
		
		mintaka_index_generate_event(event, description, pcf);
	}
	
	
	fclose(pcf);
}
