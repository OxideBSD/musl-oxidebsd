#include <sys/stat.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: OxideBSD's SYS_fchmodat takes flags natively (real fchmodat2 semantics:
 * AT_SYMLINK_NOFOLLOW on a symlink is EOPNOTSUPP), so upstream's fchmodat2-then-O_PATH-and-
 * /proc/self/fd fallback dance is unnecessary. (dirfd, path) travels as a struct __oxidebsd_at
 * pointer -- see src/internal/oxidebsd_at.h. */
int fchmodat(int fd, const char *path, mode_t mode, int flag)
{
	return syscall(SYS_fchmodat, __OXIDEBSD_AT(fd, path), mode, flag);
}
