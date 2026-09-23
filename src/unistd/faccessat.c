#include <unistd.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: OxideBSD's SYS_faccessat takes flags natively, and has no separate
 * real/effective id pair for AT_EACCESS to distinguish -- so upstream's faccessat2 attempt and its
 * fork-a-checker-with-swapped-ids fallback are both unnecessary. (dirfd, path) travels as a
 * struct __oxidebsd_at pointer -- see src/internal/oxidebsd_at.h. */
int faccessat(int fd, const char *filename, int amode, int flag)
{
	return syscall(SYS_faccessat, __OXIDEBSD_AT(fd, filename), amode, flag);
}
