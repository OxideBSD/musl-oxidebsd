/* OxideBSD: procctl(2) (OxideBSD-doc COW.md section 6.3). */
#include <sys/procctl.h>
#include "syscall.h"

int procctl(idtype_t idtype, id_t id, int cmd, void *data)
{
	return syscall(SYS_procctl, idtype, id, cmd, data);
}
