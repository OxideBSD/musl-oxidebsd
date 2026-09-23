#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: SYS_fchown was never remapped (always ENOSYS), and upstream's EBADF fallback
 * through /proc/self/fd/N doesn't resolve on OxideBSD's /proc. fchownat(fd, "", AT_EMPTY_PATH)
 * is the same operation on the fd's own inode. */
int fchown(int fd, uid_t uid, gid_t gid)
{
	return syscall(SYS_fchownat, __OXIDEBSD_AT(fd, ""), uid, gid, AT_EMPTY_PATH);
}
