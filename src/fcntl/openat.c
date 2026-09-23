#include <fcntl.h>
#include <stdarg.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: (dirfd, path) travels as a struct __oxidebsd_at pointer -- see
 * src/internal/oxidebsd_at.h for the wire format and why. */
int openat(int fd, const char *filename, int flags, ...)
{
	mode_t mode = 0;

	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	return syscall_cp(SYS_openat, __OXIDEBSD_AT(fd, filename), flags|O_LARGEFILE, mode);
}
