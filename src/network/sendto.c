#include <sys/socket.h>
#include "syscall.h"

/* OxideBSD: every send goes through sendmsg(2); the native ABI passes at most four syscall
 * arguments, fewer than sendto(2) has. */
ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t alen)
{
	struct iovec iov = { .iov_base = (void *)buf, .iov_len = len };
	struct msghdr msg = {
		.msg_name = (void *)addr, .msg_namelen = addr ? alen : 0,
		.msg_iov = &iov, .msg_iovlen = 1,
	};
	return socketcall_cp(sendmsg, fd, &msg, flags, 0, 0, 0);
}
