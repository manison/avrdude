// semaphore.h
// Copyright 2016 Robin.Rowe@CinePaint.org
// License open source MIT

#ifndef __cplusplus
#error C++11 required
#endif

#include "semaphore.h"

#include "unistd.h"
#include "../portable/stub.h"
#include <time.h>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>

class sem_c
{	unsigned flagOverwrite;
	std::mutex sem_mutex;
	std::condition_variable sem_cv;
	std::atomic<int> posts;
	bool IsPosted()
	{	const int i = posts.fetch_sub(1) - 1;
		if(i>=0)
		{
			return true;
		}
		posts.fetch_add(1);
		return false;
	}
public:
	~sem_c()
	{	flagOverwrite=0;
	}
	sem_c()
		: posts(0)
		, flagOverwrite(6009)
	{
	}
	int sem_init(int pshared, unsigned int value)
	{	if(6009 !=flagOverwrite)
		{	puts("ERROR: Windows sem_c memory overwrite");
			return -1;  
		}
		posts.exchange(value);
		return 0;
	}
	static int sem_close(sem_c *st)
	{	delete st;
		return 0;
	}
	int sem_destroy()
	{	return 0;
	}
	int sem_getvalue(sem_c *restrict, int *restrict2)
	{	*restrict2=0;
		return posts;
	}
	int sem_wait()
	{	if(IsPosted())
		{	return 0;
		}
		std::unique_lock<std::mutex> lk(sem_mutex);
		while(!IsPosted())
		{	
			sem_cv.wait(lk); // Restart if interrupted
		}
		return 0;
	}
	int sem_post()
	{	posts.fetch_add(1);
		sem_cv.notify_one();
		return 0;
	}
};

static_assert(sizeof(sem_t::_storage) >= sizeof(sem_c), "sem_t storage too small");

/*int sem_close(sem_c *st)
{	return sem_c::sem_close(st);
}

int sem_destroy(sem_c *st)
{	if(!st)
	{	return -1;
	}
	return st->sem_destroy();
}

int sem_getvalue(sem_c *st, int *val)
{	if(!st)
	{	return -1;
	}
	return st->sem_getvalue(st,val);
}*/

int sem_init(sem_t *st, int pshared, unsigned int value)
{	if(!st)
	{	return -1;
	}
	new (st->_storage) sem_c();
	return 0;
}

int sem_post(sem_t *st)
{	if(!st)
	{	return -1;
	}
	sem_c *c = reinterpret_cast<sem_c *>(st->_storage);
	return c->sem_post();
}

int sem_wait(sem_t* st)
{	if(!st)
	{	return -1;
	}
	sem_c *c = reinterpret_cast<sem_c *>(st->_storage);
	return c->sem_wait();
}

/* Linux man:

SEM_OVERVIEW(7)           Linux Programmer's Manual          SEM_OVERVIEW(7)
NAME         top

       sem_overview - overview of POSIX semaphores
DESCRIPTION         top

       POSIX semaphores allow processes and threads to synchronize their
       actions.

       A semaphore is an integer whose value is never allowed to fall below
       zero.  Two operations can be performed on semaphores: increment the
       semaphore value by one (sem_post(3)); and decrement the semaphore
       value by one (sem_wait(3)).  If the value of a semaphore is currently
       zero, then a sem_wait(3) operation will block until the value becomes
       greater than zero.

       POSIX semaphores come in two forms: named semaphores and unnamed
       semaphores.

       Named semaphores
              A named semaphore is identified by a name of the form
              /somename; that is, a null-terminated string of up to
              NAME_MAX-4 (i.e., 251) characters consisting of an initial
              slash, followed by one or more characters, none of which are
              slashes.  Two processes can operate on the same named
              semaphore by passing the same name to sem_open(3).

              The sem_open(3) function creates a new named semaphore or
              opens an existing named semaphore.  After the semaphore has
              been opened, it can be operated on using sem_post(3) and
              sem_wait(3).  When a process has finished using the semaphore,
              it can use sem_close(3) to close the semaphore.  When all
              processes have finished using the semaphore, it can be removed
              from the system using sem_unlink(3).

       Unnamed semaphores (memory-based semaphores)
              An unnamed semaphore does not have a name.  Instead the
              semaphore is placed in a region of memory that is shared
              between multiple threads (a thread-shared semaphore) or
              processes (a process-shared semaphore).  A thread-shared
              semaphore is placed in an area of memory shared between the
              threads of a process, for example, a global variable.  A
              process-shared semaphore must be placed in a shared memory
              region (e.g., a System V shared memory segment created using
              shmget(2), or a POSIX shared memory object built created using
              shm_open(3)).

              Before being used, an unnamed semaphore must be initialized
              using sem_init(3).  It can then be operated on using
              sem_post(3) and sem_wait(3).  When the semaphore is no longer
              required, and before the memory in which it is located is
              deallocated, the semaphore should be destroyed using
              sem_destroy(3).

       The remainder of this section describes some specific details of the
       Linux implementation of POSIX semaphores.

   Versions
       Prior to kernel 2.6, Linux supported only unnamed, thread-shared
       semaphores.  On a system with Linux 2.6 and a glibc that provides the
       NPTL threading implementation, a complete implementation of POSIX
       semaphores is provided.

   Persistence
       POSIX named semaphores have kernel persistence: if not removed by
       sem_unlink(3), a semaphore will exist until the system is shut down.

   Linking
       Programs using the POSIX semaphores API must be compiled with cc
       -pthread to link against the real-time library, librt.

   Accessing named semaphores via the filesystem
       On Linux, named semaphores are created in a virtual filesystem,
       normally mounted under /dev/shm, with names of the form sem.somename.
       (This is the reason that semaphore names are limited to NAME_MAX-4
       rather than NAME_MAX characters.)

       Since Linux 2.6.19, ACLs can be placed on files under this directory,
       to control object permissions on a per-user and per-group basis.
NOTES         top

       System V semaphores (semget(2), semop(2), etc.) are an older
       semaphore API.  POSIX semaphores provide a simpler, and better
       designed interface than System V semaphores; on the other hand POSIX
       semaphores are less widely available (especially on older systems)
       than System V semaphores.
EXAMPLE         top

       An example of the use of various POSIX semaphore functions is shown
       in sem_wait(3).
SEE ALSO         top

       sem_close(3), sem_destroy(3), sem_getvalue(3), sem_init(3),
       sem_open(3), sem_post(3), sem_unlink(3), sem_wait(3), pthreads(7)
COLOPHON         top

       This page is part of release 4.06 of the Linux man-pages project.  A
       description of the project, information about reporting bugs, and the
       latest version of this page, can be found at
       https://www.kernel.org/doc/man-pages/.

Linux                            2015-08-08                  SEM_OVERVIEW(7)

-----

sem_wait() decrements (locks) the semaphore pointed to by sem. If the semaphore's value is greater than zero, then the decrement proceeds, 
and the function returns, immediately. If the semaphore currently has the value zero, then the call blocks until either it becomes possible 
to perform the decrement (i.e., the semaphore value rises above zero), or a signal handler interrupts the call.

sem_crywait() is the same as sem_wait(), except that if the decrement cannot be immediately performed, then call returns an error (errno 
set to EAGAIN) instead of blocking.

sem_cimedwait() is the same as sem_wait(), except that abs_timeout specifies a limit on the amount of time that the call should block if 
the decrement cannot be immediately performed. The abs_timeout argument points to a structure that specifies an absolute timeout in seconds 
and nanoseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC). This structure is defined as follows:

struct timespec {
    time_t tv_sec;      // Seconds 
    long   tv_nsec;     // Nanoseconds [0 .. 999999999] 
};

All of these functions return 0 on success. Error is -1.

*/
