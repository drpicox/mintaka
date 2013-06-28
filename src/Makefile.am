#   $Id: Makefile.am 1591 2007-06-28 14:41:19Z drodenas $
#
#   Mintaka ParaverTrace Generator Library
#   Copyright (C) 2007 - David Rodenas Pico <david.rodenas@gmail.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
lib_LIBRARIES = libmintaka.a
bin_PROGRAMS = mintakamerge

includemintaka_HEADERS= \
	mintaka.h

includemintakadir=$(includedir)

libmintaka_a_CFLAGS=

libmintaka_a_SOURCES= \
	mintaka.c \
	mintaka-filebase.c \
	mintaka-index.c \
	mintaka-fortran.c \
	mintaka-merger.c \
	mintaka.h




mintakamerge_CPPFLAGS = $(TEST_INCDIR)

mintakamerge_SOURCES = \
	mintaka-merge.c

mintakamerge_LDADD = libmintaka.a
