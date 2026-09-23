#ifndef OXIDEBSD_AT_H
#define OXIDEBSD_AT_H

#include <string.h>

/* OxideBSD patch: the *at() family's wire format. OxideBSD's native ABI carries at most four
 * register arguments and passes paths length-prefixed (see src/fcntl/open.c), so a (dirfd, path)
 * pair doesn't fit in registers for the two-path calls -- linkat()/renameat2() would need two
 * dirfds, two pointer+length paths and flags. Every *at() syscall instead takes a pointer to one
 * of these per path, the same "small struct in caller memory" shape execve()'s argv already uses.
 * Must match RawAtPath in the OxideBSD tree's sys/modules/oxfs/src/lib.rs byte for byte.
 *
 * A NULL path is passed through as a NULL pointer (length 0) rather than faulting in strlen():
 * utimensat(fd, NULL, ...) is real, meaningful API (it's how futimens() works), and every other
 * call reports a NULL path as ENOENT kernel-side instead of crashing here. */
struct __oxidebsd_at {
	long dirfd;
	const char *path;
	unsigned long len;
};

#define __OXIDEBSD_AT(fd, p) \
	(&(struct __oxidebsd_at){ (fd), (p), (p) ? strlen(p) : 0 })

/* src/process/execve.c -- execveat() with execve()'s argv/envp conversion. */
hidden int __oxidebsd_execveat(int, const char *, char *const [], char *const [], int);

#endif
