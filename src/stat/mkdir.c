#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "syscall.h"

/* OxideBSD patch: real mkdir()'s wire format is (path, mode) in RDI/RSI. OxideBSD's own SYS_MKDIR
 * instead expects (path_ptr, path_len, mode) -- a length-prefixed pointer, no null-terminator
 * requirement -- the same argument-convention mismatch open()/chdir() needed patching for (see
 * src/fcntl/open.c and src/unistd/chdir.c on this same branch). The kernel applies the umask.
 */
int mkdir(const char *path, mode_t mode)
{
	return syscall(SYS_mkdir, path, strlen(path), mode);
}
