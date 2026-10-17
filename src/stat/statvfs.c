#include <sys/statvfs.h>
#include <sys/statfs.h>
#include <string.h>
#include "syscall.h"

/* OxideBSD patch: real statfs()'s wire format is (path, buf) -- two arguments, RDI/RSI. OxideBSD's
 * own SYS_STATFS (modules/oxfs/src/lib.rs's oxfs_statfs) instead expects (path_ptr, path_len,
 * buf_ptr) -- the same length-prefixed-path convention open()/chdir()/mkdir()/etc. already needed
 * patching for. Left unpatched, RSI (real buf) gets read as path_len and RDX (never set by a real
 * 2-arg caller) gets read as buf_ptr -- a real, confirmed crash: oxfs_statfs's own write through
 * that garbage buf_ptr page-faults. Only __statfs needs this -- __fstatfs's (fd, buf) shape has no
 * path argument to prefix, so it already matches oxfs_fstatfs's own (fd, buf_ptr) signature as-is.
 */
static int __statfs(const char *path, struct statfs *buf)
{
	*buf = (struct statfs){0};
#ifdef SYS_statfs64
	return syscall(SYS_statfs64, path, sizeof *buf, buf);
#else
	return syscall(SYS_statfs, path, strlen(path), buf);
#endif
}

static int __fstatfs(int fd, struct statfs *buf)
{
	*buf = (struct statfs){0};
#ifdef SYS_fstatfs64
	return syscall(SYS_fstatfs64, fd, sizeof *buf, buf);
#else
	return syscall(SYS_fstatfs, fd, buf);
#endif
}

weak_alias(__statfs, statfs);
weak_alias(__fstatfs, fstatfs);

/* OxideBSD: statvfs1 and fstatvfs1 fill struct statvfs whole, with the file system's type and
 * mount names (NetBSD's shape); getvfsstat lists every mount. */
int statvfs(const char *restrict path, struct statvfs *restrict buf)
{
	return syscall(SYS_statvfs1, path, strlen(path), buf);
}

int fstatvfs(int fd, struct statvfs *buf)
{
	return syscall(SYS_fstatvfs1, fd, buf);
}

int getvfsstat(struct statvfs *buf, size_t bufsize, int flags)
{
	return syscall(SYS_getvfsstat, buf, bufsize, flags);
}
