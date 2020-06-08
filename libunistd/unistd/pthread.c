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

#include "pthread.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <crtdbg.h>
#include <stdbool.h>

static int cancelled;

int pthread_create(pthread_t *pthread, const pthread_attr_t *attr, unsigned (__stdcall * start_routine)(void *), void *arg)
{
	_ASSERTE(attr == NULL);
	_ASSERTE(cancelled == 0);

	cancelled = -1;
	DWORD tid;
	*pthread = CreateThread(NULL, 0, start_routine, arg, 0, &tid);
	return *pthread != NULL ? 0 : -1;
}

int pthread_join(pthread_t thread, void **retval)
{
	_ASSERTE(retval == NULL);
	WaitForSingleObject((HANDLE)thread, INFINITE);
	cancelled = 0;
	return 0;
}

int pthread_cancel(pthread_t thread)
{
	_ASSERTE(cancelled == -1);
	cancelled = 1;
	return 0;
}

void pthread_testcancel(void)
{
	if (cancelled == 1)
	{
		ExitThread(0);
	}
}
