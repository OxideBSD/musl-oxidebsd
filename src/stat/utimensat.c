#include <sys/stat.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: this used to support only AT_FDCWD (ENOSYS for any real dirfd, which also made
 * futimens(fd) -- utimensat(fd, NULL, ...) -- always fail), going through the older path-only
 * SYS_UTIMENSAT. utimensat now maps to OxideBSD's real, dirfd-aware SYS_UTIMENSAT_AT; a NULL path
 * reaches the kernel as a NULL pointer, which means "the fd itself". See
 * src/internal/oxidebsd_at.h. */
int utimensat(int fd, const char *path, const struct timespec times[2], int flags)
{
	return syscall(SYS_utimensat, __OXIDEBSD_AT(fd, path), times, flags);
}
