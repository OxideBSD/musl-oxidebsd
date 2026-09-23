#include <unistd.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
ssize_t readlinkat(int fd, const char *restrict path, char *restrict buf, size_t bufsize)
{
	char dummy[1];
	if (!bufsize) {
		buf = dummy;
		bufsize = 1;
	}
	int r = __syscall(SYS_readlinkat, __OXIDEBSD_AT(fd, path), buf, bufsize);
	if (buf == dummy && r > 0) r = 0;
	return __syscall_ret(r);
}
