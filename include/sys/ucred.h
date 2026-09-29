#ifndef _SYS_UCRED_H
#define _SYS_UCRED_H

/* OxideBSD: FreeBSD's struct xucred, what getsockopt(SOL_LOCAL, LOCAL_PEERCRED) returns about a
 * local socket's peer (UNIX.md §9.2). */

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#define __NEED_uid_t
#define __NEED_gid_t
#define __NEED_pid_t
#include <bits/alltypes.h>

#define XU_NGROUPS 16
#define XUCRED_VERSION 0

struct xucred {
	unsigned cr_version;
	uid_t cr_uid;
	short cr_ngroups;
	gid_t cr_groups[XU_NGROUPS];
	union {
		void *_cr_unused1;
		pid_t cr_pid;
	};
};

#ifdef __cplusplus
}
#endif

#endif
