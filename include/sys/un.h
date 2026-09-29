#ifndef	_SYS_UN_H
#define	_SYS_UN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#define __NEED_sa_family_t
#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define __NEED_size_t
#endif

#include <bits/alltypes.h>

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[108];
};

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
size_t strlen(const char *);
#define SUN_LEN(s) (2+strlen((s)->sun_path))

/* OxideBSD: local-socket options (UNIX.md §9), FreeBSD's names. The values are OxideBSD's own:
 * FreeBSD's SOL_LOCAL is 0, which is SOL_IP here, and its LOCAL_* options are 1-3, which are
 * SO_* values here. */
#define SOL_LOCAL               0x200
#define LOCAL_PEERCRED          0x1001
#define LOCAL_CREDS             0x1002
#define LOCAL_CREDS_PERSISTENT  0x1003
#endif

#ifdef __cplusplus
}
#endif

#endif
