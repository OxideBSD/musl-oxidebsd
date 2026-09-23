#include <unistd.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
int unlinkat(int fd, const char *path, int flag)
{
	return syscall(SYS_unlinkat, __OXIDEBSD_AT(fd, path), flag);
}
