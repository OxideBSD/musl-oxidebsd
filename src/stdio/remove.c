#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: upstream calls SYS_unlink/SYS_rmdir with the real one-argument shape, but
 * OxideBSD's versions take (path_ptr, path_len) -- the length landed as whatever garbage was left
 * in RSI. Routed through unlinkat instead (the same thing upstream does on arches without
 * SYS_unlink), whose (dirfd, path) is a struct __oxidebsd_at pointer. */
int remove(const char *path)
{
	int r = __syscall(SYS_unlinkat, __OXIDEBSD_AT(AT_FDCWD, path), 0);
	if (r==-EISDIR) r = __syscall(SYS_unlinkat, __OXIDEBSD_AT(AT_FDCWD, path), AT_REMOVEDIR);
	return __syscall_ret(r);
}
