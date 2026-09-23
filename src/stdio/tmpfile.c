#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "stdio_impl.h"
#include "oxidebsd_at.h"

#define MAXTRIES 100

FILE *tmpfile(void)
{
	char s[] = "/tmp/tmpfile_XXXXXX";
	int fd;
	FILE *f;
	int try;
	for (try=0; try<MAXTRIES; try++) {
		__randname(s+13);
		fd = sys_open(s, O_RDWR|O_CREAT|O_EXCL, 0600);
		if (fd >= 0) {
			/* OxideBSD patch: SYS_unlink takes (path_ptr, path_len) on OxideBSD, so upstream's
			 * one-argument call sent a garbage length -- see src/stdio/remove.c. */
			__syscall(SYS_unlinkat, __OXIDEBSD_AT(AT_FDCWD, s), 0);
			f = __fdopen(fd, "w+");
			if (!f) __syscall(SYS_close, fd);
			return f;
		}
	}
	return 0;
}
