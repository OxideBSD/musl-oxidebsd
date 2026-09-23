#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "syscall.h"
#include "oxidebsd_at.h"

#define MAXTRIES 100

char *tmpnam(char *buf)
{
	static char internal[L_tmpnam];
	char s[] = "/tmp/tmpnam_XXXXXX";
	int try;
	int r;
	for (try=0; try<MAXTRIES; try++) {
		__randname(s+12);
		/* OxideBSD patch: SYS_readlink takes (path_ptr, path_len, buf, bufsize) on OxideBSD, so
		 * upstream's three-argument call misread every argument -- see src/stdio/remove.c. */
		r = __syscall(SYS_readlinkat, __OXIDEBSD_AT(AT_FDCWD, s), (char[1]){0}, 1);
		if (r == -ENOENT) return strcpy(buf ? buf : internal, s);
	}
	return 0;
}
