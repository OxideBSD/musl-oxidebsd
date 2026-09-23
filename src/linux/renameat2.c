#define _GNU_SOURCE
#include <stdio.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
int renameat2(int oldfd, const char *old, int newfd, const char *new, unsigned flags)
{
	return syscall(SYS_renameat2, __OXIDEBSD_AT(oldfd, old), __OXIDEBSD_AT(newfd, new), flags);
}
