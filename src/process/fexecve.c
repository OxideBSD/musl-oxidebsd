#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"
#include "oxidebsd_at.h"

/* OxideBSD patch: OxideBSD has a real execveat(AT_EMPTY_PATH), so upstream's /proc/self/fd/N
 * fallback (which doesn't resolve to the file on OxideBSD's /proc) is dropped. The argv/envp
 * conversion lives in src/process/execve.c's __oxidebsd_execveat(). */
int fexecve(int fd, char *const argv[], char *const envp[])
{
	return __oxidebsd_execveat(fd, "", argv, envp, AT_EMPTY_PATH);
}
