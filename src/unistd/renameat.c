#include <stdio.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
int renameat(int oldfd, const char *old, int newfd, const char *new)
{
	return syscall(SYS_renameat, __OXIDEBSD_AT(oldfd, old), __OXIDEBSD_AT(newfd, new));
}
