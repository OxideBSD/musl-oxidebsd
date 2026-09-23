#include <sys/stat.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: this used to route through plain SYS_mknod and return ENOSYS for any real
 * dirfd, back when OxideBSD had no dirfd-relative resolution. It has a real SYS_mknodat now;
 * (dirfd, path) travels as a struct __oxidebsd_at pointer -- see src/internal/oxidebsd_at.h. */
int mknodat(int fd, const char *path, mode_t mode, dev_t dev)
{
	return syscall(SYS_mknodat, __OXIDEBSD_AT(fd, path), mode, dev);
}
