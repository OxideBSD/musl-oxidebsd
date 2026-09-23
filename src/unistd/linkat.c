#include <unistd.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
int linkat(int fd1, const char *existing, int fd2, const char *new, int flag)
{
	return syscall(SYS_linkat, __OXIDEBSD_AT(fd1, existing), __OXIDEBSD_AT(fd2, new), flag);
}
