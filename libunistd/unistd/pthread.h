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

#ifndef pthread_h
#define pthread_h

typedef void * pthread_t;
typedef void * pthread_attr_t;

int pthread_create(pthread_t *pthread, const pthread_attr_t *attr, unsigned (__stdcall * start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);

#define PTHREAD_CANCEL_ASYNCHRONOUS 1

static inline int pthread_setcanceltype(int type, int *oldtype)
{
	return 0;
}

int pthread_cancel(pthread_t thread);
void pthread_testcancel(void);

#endif
