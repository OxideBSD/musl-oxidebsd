#ifndef _SYS_PROCCTL_H
#define _SYS_PROCCTL_H

/* OxideBSD: procctl(2), per-process controls (OxideBSD-doc COW.md section 6.3). FreeBSD's
 * interface; the commands and their numbers are OxideBSD's own. */

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/wait.h>

/* The OOM killer's adjustment of a process, -1000 to 1000 (data: int *). */
#define PROC_OOM_ADJ_CTL	1
#define PROC_OOM_ADJ_STATUS	2

#define OOM_ADJ_MIN	(-1000)
#define OOM_ADJ_MAX	1000

int procctl(idtype_t, id_t, int, void *);

#ifdef __cplusplus
}
#endif

#endif
