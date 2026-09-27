#define _GNU_SOURCE
#include <utmpx.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

/* OxideBSD: real utmpx(3) over NetBSD's file names (LOGIN.md §8 in
 * OxideBSD-doc). Stock musl stubs every function out. Records are this
 * header's struct utmpx, written whole: /var/run/utmpx holds one record per
 * terminal (current sessions), /var/log/wtmpx is an append-only history. */

#define _PATH_UTMPX "/var/run/utmpx"

static char path[PATH_MAX] = _PATH_UTMPX;
static int fd = -1;
static int writable;
static struct utmpx ent;

static int open_db(int want_write)
{
	if (fd >= 0 && (writable || !want_write)) return 0;
	if (fd >= 0) close(fd);
	writable = 1;
	fd = open(path, O_RDWR|O_CLOEXEC|(want_write ? O_CREAT : 0), 0644);
	if (fd < 0 && !want_write) {
		writable = 0;
		fd = open(path, O_RDONLY|O_CLOEXEC);
	}
	return fd < 0 ? -1 : 0;
}

void endutxent(void)
{
	if (fd >= 0) close(fd);
	fd = -1;
}

void setutxent(void)
{
	if (fd >= 0) lseek(fd, 0, SEEK_SET);
}

struct utmpx *getutxent(void)
{
	if (open_db(0) < 0) return NULL;
	if (read(fd, &ent, sizeof ent) != sizeof ent) return NULL;
	return &ent;
}

static int is_session(short type)
{
	return type == INIT_PROCESS || type == LOGIN_PROCESS
		|| type == USER_PROCESS || type == DEAD_PROCESS;
}

/* POSIX's matching rule for getutxid(). */
static int id_matches(const struct utmpx *a, const struct utmpx *b)
{
	switch (a->ut_type) {
	case BOOT_TIME: case OLD_TIME: case NEW_TIME:
		return b->ut_type == a->ut_type;
	case INIT_PROCESS: case LOGIN_PROCESS: case USER_PROCESS: case DEAD_PROCESS:
		return is_session(b->ut_type)
			&& !strncmp(a->ut_id, b->ut_id, sizeof a->ut_id);
	}
	return 0;
}

struct utmpx *getutxid(const struct utmpx *ut)
{
	struct utmpx key = *ut, *e;
	while ((e = getutxent()))
		if (id_matches(&key, e)) return e;
	return NULL;
}

struct utmpx *getutxline(const struct utmpx *ut)
{
	struct utmpx key = *ut, *e;
	while ((e = getutxent()))
		if ((e->ut_type == LOGIN_PROCESS || e->ut_type == USER_PROCESS)
		    && !strncmp(key.ut_line, e->ut_line, sizeof key.ut_line))
			return e;
	return NULL;
}

/* Replaces the record getutxid() would find, or appends one. The whole file
 * is searched, not just from the current position: callers (login, init)
 * never rely on the position, and a stale earlier record must not be left
 * next to a new one for the same terminal. */
struct utmpx *pututxline(const struct utmpx *ut)
{
	struct utmpx rec = *ut, *e;
	off_t at = -1;

	if (open_db(1) < 0) return NULL;
	lseek(fd, 0, SEEK_SET);
	while ((e = getutxent())) {
		if (id_matches(&rec, e)) {
			at = lseek(fd, 0, SEEK_CUR) - (off_t)sizeof rec;
			break;
		}
	}
	if (at < 0) at = lseek(fd, 0, SEEK_END);
	if (at < 0 || pwrite(fd, &rec, sizeof rec, at) != sizeof rec) return NULL;
	lseek(fd, at + (off_t)sizeof rec, SEEK_SET);
	ent = rec;
	return &ent;
}

void updwtmpx(const char *f, const struct utmpx *u)
{
	int wfd = open(f, O_WRONLY|O_APPEND|O_CREAT|O_CLOEXEC, 0644);
	if (wfd < 0) return;
	write(wfd, u, sizeof *u);
	close(wfd);
}

static int __utmpxname(const char *f)
{
	size_t l = strlen(f);
	if (l >= sizeof path) {
		errno = ENAMETOOLONG;
		return -1;
	}
	endutxent();
	memcpy(path, f, l + 1);
	return 0;
}

weak_alias(endutxent, endutent);
weak_alias(setutxent, setutent);
weak_alias(getutxent, getutent);
weak_alias(getutxid, getutid);
weak_alias(getutxline, getutline);
weak_alias(pututxline, pututline);
weak_alias(updwtmpx, updwtmp);
weak_alias(__utmpxname, utmpname);
weak_alias(__utmpxname, utmpxname);
