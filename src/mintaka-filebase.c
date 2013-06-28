/*
    $Id: mintaka-filebase.c 1591 2007-06-28 14:41:19Z drodenas $
    
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


#include <assert.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>



static const char* mintaka__filebase;
static const char mintaka__fileorigin[4096];

/**
 * Utility function for filebase. 
 * It sets the tracefile filebase if it is already no assigned.
 * It uses the POSIX 'basename' to remove the path.
 */
void 
mintaka_set_filebase(const char* tracefile_filebase)
{
	const char* nopath_filebase;
	char* copy= strdup(tracefile_filebase);
	
	nopath_filebase= basename(copy);
	mintaka_set_fullpath_filebase(nopath_filebase);
}

/**
 * Utility function for filebase. 
 * It sets the tracefile filebase if it is already no assigned.
 */
void 
mintaka_set_fullpath_filebase(const char* fullpath_tracefile_filebase)
{
	assert(fullpath_tracefile_filebase);
	assert(mintaka__filebase == NULL);
	
	mintaka__filebase= fullpath_tracefile_filebase;
}

/**
 * Utility function for query filebase.
 */
const char* 
mintaka_get_filebase()
{
	if (mintaka__filebase == NULL)
	{
		const char* env= getenv("MINTAKA_FILEBASE");
		if (env != NULL)
		{
			mintaka_set_fullpath_filebase(env);
		}
		else
		{
			mintaka_set_fullpath_filebase("trace");
		}
	}

	return mintaka__filebase;
}
