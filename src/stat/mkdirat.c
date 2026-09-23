#include <sys/stat.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
int mkdirat(int fd, const char *path, mode_t mode)
{
	return syscall(SYS_mkdirat, __OXIDEBSD_AT(fd, path), mode);
}
