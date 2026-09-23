#include <unistd.h>
#include <string.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: only the new link's own path is dirfd-relative -- the target is stored content,
 * so it stays a plain (pointer, length) pair. See src/internal/oxidebsd_at.h. */
int symlinkat(const char *existing, int fd, const char *new)
{
	return syscall(SYS_symlinkat, existing, strlen(existing), __OXIDEBSD_AT(fd, new));
}
