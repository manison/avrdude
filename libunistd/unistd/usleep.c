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

static LARGE_INTEGER freq;

int usleep(unsigned usec)
{
	if (usec > 10000) {
		Sleep(usec / 1000);
		return 0;
	}

	// Adapted from bitbang.c

	if (freq.QuadPart == 0) {
		QueryPerformanceFrequency(&freq);
	}

	LARGE_INTEGER now, end;
	QueryPerformanceCounter(&now);
	end.QuadPart = now.QuadPart + freq.QuadPart * usec / 1000000LL;

	while (now.QuadPart < end.QuadPart) {
		YieldProcessor();
		QueryPerformanceCounter(&now);
	}

	return 0;
}
