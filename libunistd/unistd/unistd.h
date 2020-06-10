/*
* avrdude - A Downloader/Uploader for AVR device programmers
* Copyright (C) 2020 manison <manison@manison.cz>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef unistd_h
#define unistd_h

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <io.h>

#define PATH_MAX	MAX_PATH

int usleep(unsigned usec);

#define strcasecmp	stricmp
#define strncasecmp	strnicmp

#define isatty		_isatty
#define STDIN_FILENO	_fileno(stdin)

extern int opterr, optind, optopt, optreset;
extern char *optarg;
int getopt(int nargc, char * const nargv[], const char *ostr);

#endif
