#include <sys/socket.h>
#include "syscall.h"

/* OxideBSD: every receive goes through recvmsg(2); the native ABI passes at most four syscall
 * arguments, fewer than recvfrom(2) has. */
ssize_t recvfrom(int fd, void *restrict buf, size_t len, int flags, struct sockaddr *restrict addr, socklen_t *restrict alen)
{
	struct iovec iov = { .iov_base = buf, .iov_len = len };
	struct msghdr msg = {
		.msg_name = addr, .msg_namelen = addr && alen ? *alen : 0,
		.msg_iov = &iov, .msg_iovlen = 1,
	};
	ssize_t r = socketcall_cp(recvmsg, fd, &msg, flags, 0, 0, 0);
	if (r >= 0 && addr && alen) *alen = msg.msg_namelen;
	return r;
}
