#ifndef OXIDEBSD_SOCKOPT_H
#define OXIDEBSD_SOCKOPT_H

/* OxideBSD's getsockopt(2)/setsockopt(2) take (fd, args): the native ABI passes at most four
 * syscall arguments, and these have five. For getsockopt, len is a socklen_t *; for setsockopt,
 * the option's length. (The compound literal is parenthesized: __syscall counts its arguments,
 * and braces don't hide commas from the preprocessor.) */
struct __oxidebsd_sockopt {
	long level, name;
	const void *val;
	unsigned long len;
};

#define __oxidebsd_getsockopt(fd, level, name, val, lenp) \
	__syscall(SYS_getsockopt, fd, (&(struct __oxidebsd_sockopt){ level, name, val, (unsigned long)(lenp) }))
#define __oxidebsd_setsockopt(fd, level, name, val, len) \
	__syscall(SYS_setsockopt, fd, (&(struct __oxidebsd_sockopt){ level, name, val, len }))

#endif
