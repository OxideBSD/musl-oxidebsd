#include <semaphore.h>
#include <limits.h>
#include <errno.h>
#include "syscall.h"
#include "atomic.h"

/* OxideBSD fix: real, unmodified musl advertises a fixed SEM_NSEMS_MAX via
 * sysconf(_SC_SEM_NSEMS_MAX) (see include/limits.h) but never actually
 * enforces it -- sem_init() is pure userspace bookkeeping with no syscall
 * involved, so nothing ever returns ENOSPC once the limit is exceeded
 * (POSIX conformance test sem_init/7-1.c). Track a real, honest running
 * count across every unnamed semaphore currently initialized so the
 * advertised limit means something. */
hidden int __sem_nsems_open;

int sem_init(sem_t *sem, int pshared, unsigned value)
{
	if (value > SEM_VALUE_MAX) {
		errno = EINVAL;
		return -1;
	}
	if (a_fetch_add(&__sem_nsems_open, 1) >= SEM_NSEMS_MAX) {
		a_fetch_add(&__sem_nsems_open, -1);
		errno = ENOSPC;
		return -1;
	}
	sem->__val[0] = value;
	sem->__val[1] = 0;
	sem->__val[2] = pshared ? 0 : 128;
	return 0;
}
