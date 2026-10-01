#include <sys/mount.h>
#include <sys/uio.h>
#include <string.h>
#include <errno.h>
#include "syscall.h"

/* OxideBSD patch: nmount(2), FreeBSD's mount interface (OxideBSD's number, 584): name/value
 * options in an iovec array, one call for every file system type. See nmount(2) in the OxideBSD
 * tree (share/man/man2/nmount.2). */
int nmount(struct iovec *iov, unsigned niov, int flags)
{
	return syscall(SYS_nmount, iov, niov, flags);
}

/* OxideBSD patch: mount(2) on nmount(2). The two shapes the kernel has: a bind mount
 * (MS_BIND, which is nullfs), and `-t tmpfs`. Flags other than MS_BIND, and data, are ignored, as
 * the kernel takes no options yet; any other file system type is ENODEV. */
int mount(const char *special, const char *dir, const char *fstype, unsigned long flags, const void *data)
{
	struct iovec iov[6];
	unsigned n = 0;
	(void)data;
#define OPT(name, value) do { \
		iov[n].iov_base = (void *)(name); iov[n++].iov_len = strlen(name) + 1; \
		iov[n].iov_base = (void *)(value); iov[n++].iov_len = strlen(value) + 1; \
	} while (0)
	if (fstype && !strcmp(fstype, "tmpfs")) {
		OPT("fstype", "tmpfs");
	} else if (flags & MS_BIND) {
		OPT("fstype", "nullfs");
		OPT("from", special);
	} else {
		errno = ENODEV;
		return -1;
	}
	OPT("fspath", dir);
#undef OPT
	return nmount(iov, n, 0);
}

/* OxideBSD patch: real umount()/umount2()'s own (special[, flags]) wire format fits this ABI's 4
 * registers whole once path_len is added -- the same "compute strlen() explicitly" patch
 * chown()/rename() already use, no shape change needed otherwise. Reuses the real delete_module
 * syscall slot (176), see arch/x86_64/bits/syscall.h.in -- this file's own SYS_umount2
 * macro stays at its original, inert real-Linux value (166), unreferenced from here on.
 */
int umount(const char *special)
{
	long ret = __syscall4(SYS_delete_module, (long)special, (long)strlen(special), 0, 0);
	return __syscall_ret(ret);
}

int umount2(const char *special, int flags)
{
	long ret = __syscall4(SYS_delete_module, (long)special, (long)strlen(special), (long)flags, 0);
	return __syscall_ret(ret);
}
