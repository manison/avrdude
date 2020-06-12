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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sys/time.h>
#include <crtdbg.h>

#define EPOCH_START_USEC  11644473600000000ULL

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	_ASSERTE(tz == NULL);

	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);

	ULARGE_INTEGER ul;
	ul.HighPart = ft.dwHighDateTime;
	ul.LowPart = ft.dwLowDateTime;

	ul.QuadPart /= 10; // 100ns units -> microseconds
	ul.QuadPart -= EPOCH_START_USEC;
	
	tv->tv_sec  = (time_t)(ul.QuadPart / 1000000UL);
	tv->tv_usec =         (ul.QuadPart % 1000000UL);

	return 0;
}
