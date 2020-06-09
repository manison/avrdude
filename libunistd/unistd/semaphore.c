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

#include "semaphore.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <crtdbg.h>
#include <stdbool.h>
#include <limits.h>

 // This is absolutely minimum and naive implementation of the POSIX semaphore API for making the ft245 programmer work.

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
	_ASSERTE(sem != NULL);
	_ASSERTE(pshared == 0);

	InitializeConditionVariable(&sem->cv);
	InitializeCriticalSectionEx(&sem->mutex, 4000, 0);
	sem->count = value;

	return 0;
}

int sem_post(sem_t *sem)
{
	_ASSERTE(sem != NULL);

	EnterCriticalSection(&sem->mutex);
	++sem->count;
	WakeConditionVariable(&sem->cv);
	LeaveCriticalSection(&sem->mutex);

	return 0;
}

int sem_wait(sem_t *sem)
{
	_ASSERTE(sem != NULL);

	EnterCriticalSection(&sem->mutex);
	while (sem->count == 0) {
		SleepConditionVariableCS(&sem->cv, &sem->mutex, INFINITE);
	}
	--sem->count;
	LeaveCriticalSection(&sem->mutex);

	return 0;
}
