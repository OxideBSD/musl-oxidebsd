#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: SYS_lchown was never remapped (always ENOSYS); OxideBSD implements it as
 * fchownat(AT_SYMLINK_NOFOLLOW), the same thing upstream does on arches without SYS_lchown. */
int lchown(const char *path, uid_t uid, gid_t gid)
{
	return syscall(SYS_fchownat, __OXIDEBSD_AT(AT_FDCWD, path), uid, gid, AT_SYMLINK_NOFOLLOW);
}
