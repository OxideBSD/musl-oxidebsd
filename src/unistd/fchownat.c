#include <unistd.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flag)
{
	return syscall(SYS_fchownat, __OXIDEBSD_AT(fd, path), uid, gid, flag);
}
