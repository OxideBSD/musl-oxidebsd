#ifndef _SYS_SYSCTL_H
#define _SYS_SYSCTL_H

/* OxideBSD: the kernel's management information base, FreeBSD's interface (OxideBSD-doc
 * SYSCTL.md). Numbers are FreeBSD's. */

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#define __NEED_size_t
#include <bits/alltypes.h>

#define CTL_MAXNAME	24

#define CTLTYPE		0xf
#define CTLTYPE_NODE	1
#define CTLTYPE_INT	2
#define CTLTYPE_STRING	3
#define CTLTYPE_S64	4
#define CTLTYPE_OPAQUE	5
#define CTLTYPE_STRUCT	CTLTYPE_OPAQUE
#define CTLTYPE_UINT	6
#define CTLTYPE_LONG	7
#define CTLTYPE_ULONG	8
#define CTLTYPE_U64	9
#define CTLTYPE_U8	0xa
#define CTLTYPE_U16	0xb
#define CTLTYPE_S8	0xc
#define CTLTYPE_S16	0xd
#define CTLTYPE_S32	0xe
#define CTLTYPE_U32	0xf

#define CTLFLAG_RD	0x80000000
#define CTLFLAG_WR	0x40000000
#define CTLFLAG_RW	(CTLFLAG_RD|CTLFLAG_WR)
#define CTLFLAG_TUN	0x00080000
#define CTLFLAG_RDTUN	(CTLFLAG_RD|CTLFLAG_TUN)

#define CTL_UNSPEC	0
#define CTL_KERN	1
#define CTL_VM		2
#define CTL_VFS		3
#define CTL_NET		4
#define CTL_DEBUG	5
#define CTL_HW		6
#define CTL_MACHDEP	7
#define CTL_USER	8

#define KERN_OSTYPE		1
#define KERN_OSRELEASE		2
#define KERN_OSREV		3
#define KERN_VERSION		4
#define KERN_MAXVNODES		5
#define KERN_MAXPROC		6
#define KERN_MAXFILES		7
#define KERN_ARGMAX		8
#define KERN_SECURELVL		9
#define KERN_HOSTNAME		10
#define KERN_HOSTID		11
#define KERN_CLOCKRATE		12
#define KERN_PROC		14
#define KERN_FILE		15
#define KERN_PROF		16
#define KERN_POSIX1		17
#define KERN_NGROUPS		18
#define KERN_JOB_CONTROL	19
#define KERN_SAVED_IDS		20
#define KERN_BOOTTIME		21
#define KERN_NISDOMAINNAME	22
#define KERN_UPDATEINTERVAL	23
#define KERN_OSRELDATE		24
#define KERN_BOOTFILE		26
#define KERN_MAXFILESPERPROC	27
#define KERN_MAXPROCPERUID	28
#define KERN_IOV_MAX		35
#define KERN_HOSTUUID		36
#define KERN_ARND		37

#define VM_TOTAL	1
#define VM_METER	VM_TOTAL
#define VM_LOADAVG	2

#define HW_MACHINE	1
#define HW_MODEL	2
#define HW_NCPU		3
#define HW_BYTEORDER	4
#define HW_PHYSMEM	5
#define HW_USERMEM	6
#define HW_PAGESIZE	7
#define HW_FLOATINGPT	10
#define HW_MACHINE_ARCH	11
#define HW_REALMEM	12

/* kern.clockrate */
struct clockinfo {
	int hz;
	int tick;
	int spare;
	int stathz;
	int profhz;
};

int sysctl(const int *, unsigned int, void *, size_t *, const void *, size_t);
int sysctlbyname(const char *, void *, size_t *, const void *, size_t);
int sysctlnametomib(const char *, int *, size_t *);

#ifdef __cplusplus
}
#endif

#endif
