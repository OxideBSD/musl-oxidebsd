#include <semaphore.h>
#include "syscall.h"
#include "atomic.h"

/* See sem_init.c -- the counter this decrements. */
extern hidden int __sem_nsems_open;

int sem_destroy(sem_t *sem)
{
	int n;
	do {
		n = __sem_nsems_open;
	} while (n > 0 && a_cas(&__sem_nsems_open, n, n-1) != n);
	return 0;
}
