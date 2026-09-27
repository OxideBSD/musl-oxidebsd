#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pthread.h>
#include "pwf.h"

/* OxideBSD: getspnam(3) reads /etc/master.passwd, the BSD account file
 * (LOGIN.md §7.4 in OxideBSD-doc); there is no /etc/shadow. Stock musl read
 * /etc/shadow, or an Openwall TCB file. Like stock musl, it allocates
 * nothing, so a huge file can't exhaust memory. */

static long xatol(char **s)
{
	long x;
	if (**s == ':' || **s == '\n') return -1;
	for (x=0; **s-'0'<10U; ++*s) x=10*x+(**s-'0');
	return x;
}

int __parsespent(char *s, struct spwd *sp)
{
	sp->sp_namp = s;
	if (!(s = strchr(s, ':'))) return -1;
	*s = 0;

	sp->sp_pwdp = ++s;
	if (!(s = strchr(s, ':'))) return -1;
	*s = 0;

	s++; sp->sp_lstchg = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_min = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_max = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_warn = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_inact = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_expire = xatol(&s);
	if (*s != ':') return -1;

	s++; sp->sp_flag = xatol(&s);
	if (*s != '\n') return -1;
	return 0;
}

/* One master.passwd(5) line, name:password:uid:gid:class:change:expire:
 * gecos:home:shell, as a struct spwd. BSD's change field is the time by which
 * the password must be changed, so it becomes sp_lstchg with sp_max 0: the
 * shadow expiry date, sp_lstchg + sp_max, is that time. Both fields are
 * seconds in master.passwd and days in struct spwd; 0 means none (-1). */
static int parse_master(char *s, struct spwd *sp)
{
	long change, expire;
	int i;

	sp->sp_namp = s;
	if (!(s = strchr(s, ':'))) return -1;
	*s++ = 0;
	sp->sp_pwdp = s;
	if (!(s = strchr(s, ':'))) return -1;
	*s++ = 0;
	/* uid, gid, class */
	for (i = 0; i < 3; i++)
		if (!(s = strchr(s, ':'))) return -1;
		else s++;
	change = xatol(&s);
	if (*s++ != ':') return -1;
	expire = xatol(&s);
	if (*s != ':') return -1;

	sp->sp_lstchg = change > 0 ? change / 86400 : -1;
	sp->sp_max = change > 0 ? 0 : -1;
	sp->sp_min = sp->sp_warn = sp->sp_inact = -1;
	sp->sp_expire = expire > 0 ? expire / 86400 : -1;
	sp->sp_flag = -1;
	return 0;
}

static void cleanup(void *p)
{
	fclose(p);
}

int getspnam_r(const char *name, struct spwd *sp, char *buf, size_t size, struct spwd **res)
{
	FILE *f;
	int rv = 0;
	size_t k, l = strlen(name);
	int skip = 0;
	int orig_errno = errno;

	*res = 0;

	if (!l || strchr(name, ':'))
		return errno = EINVAL;

	/* Buffer size must at least be able to hold name, plus some.. */
	if (size < l+100)
		return errno = ERANGE;

	f = fopen("/etc/master.passwd", "rbe");
	if (!f) {
		if (errno != ENOENT && errno != ENOTDIR)
			return errno;
		errno = orig_errno;
		return 0;
	}

	pthread_cleanup_push(cleanup, f);
	while (fgets(buf, size, f) && (k=strlen(buf))>0) {
		if (skip || strncmp(name, buf, l) || buf[l]!=':') {
			skip = buf[k-1] != '\n';
			continue;
		}
		if (buf[k-1] != '\n') {
			rv = ERANGE;
			break;
		}

		if (parse_master(buf, sp) < 0) continue;
		*res = sp;
		break;
	}
	pthread_cleanup_pop(1);
	errno = rv ? rv : orig_errno;
	return rv;
}
