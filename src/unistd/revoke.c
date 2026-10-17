#include <unistd.h>
#include <string.h>
#include "syscall.h"

/* OxideBSD's revoke(2) (OxideBSD-doc VFS.md 4.4): every descriptor of the file goes dead. Paths
 * are length-prefixed in this ABI. */
int revoke(const char *path)
{
	return syscall(SYS_revoke, path, strlen(path));
}
