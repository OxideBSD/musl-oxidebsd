/* OxideBSD: getpeereid(3), as the BSDs have it, over LOCAL_PEERCRED (UNIX.md §9.2). */
#define _BSD_SOURCE
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ucred.h>

int getpeereid(int s, uid_t *euid, gid_t *egid)
{
	struct xucred xuc;
	socklen_t len = sizeof xuc;
	if (getsockopt(s, SOL_LOCAL, LOCAL_PEERCRED, &xuc, &len) < 0) return -1;
	if (len != sizeof xuc || xuc.cr_version != XUCRED_VERSION) {
		errno = EINVAL;
		return -1;
	}
	*euid = xuc.cr_uid;
	*egid = xuc.cr_groups[0];
	return 0;
}
