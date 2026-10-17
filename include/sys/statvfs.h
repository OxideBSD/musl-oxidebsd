#ifndef	_SYS_STATVFS_H
#define	_SYS_STATVFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#define __NEED_fsblkcnt_t
#define __NEED_fsfilcnt_t
#define __NEED_size_t
#include <bits/alltypes.h>

struct statvfs {
	unsigned long f_bsize, f_frsize;
	fsblkcnt_t f_blocks, f_bfree, f_bavail;
	fsfilcnt_t f_files, f_ffree, f_favail;
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned long f_fsid;
	unsigned :8*(2*sizeof(int)-sizeof(long));
#else
	unsigned :8*(2*sizeof(int)-sizeof(long));
	unsigned long f_fsid;
#endif
	unsigned long f_flag, f_namemax;
	unsigned int f_type;
	int __reserved[5];
	/* OxideBSD, as NetBSD's statvfs (POSIX allows more members): the mounted file system's type,
	 * where it is mounted and what from. */
	char f_fstypename[32];
	char f_mntonname[1024];
	char f_mntfromname[1024];
};

#define VFS_NAMELEN 32
#define VFS_MNAMELEN 1024

int statvfs (const char *__restrict, struct statvfs *__restrict);
int fstatvfs (int, struct statvfs *);

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
/* NetBSD's: every mounted file system, as many as bufsize bytes hold (all of them counted when
 * buf is null); ST_WAIT or ST_NOWAIT. The number of records. */
int getvfsstat(struct statvfs *, size_t, int);
#define ST_WAIT   1
#define ST_NOWAIT 2
#endif

#define ST_RDONLY 1
#define ST_NOSUID 2
#define ST_NODEV  4
#define ST_NOEXEC 8
#define ST_SYNCHRONOUS 16
#define ST_MANDLOCK    64
#define ST_WRITE       128
#define ST_APPEND      256
#define ST_IMMUTABLE   512
#define ST_NOATIME     1024
#define ST_NODIRATIME  2048
#define ST_RELATIME    4096

#if defined(_LARGEFILE64_SOURCE)
#define statvfs64 statvfs
#define fstatvfs64 fstatvfs
#define fsblkcnt64_t fsblkcnt_t
#define fsfilcnt64_t fsfilcnt_t
#endif

#ifdef __cplusplus
}
#endif

#endif
