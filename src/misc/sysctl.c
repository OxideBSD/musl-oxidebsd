/* OxideBSD: sysctl(3), sysctlbyname(3), sysctlnametomib(3) over sysctl(2) (OxideBSD-doc
 * SYSCTL.md §4). The native ABI passes four arguments, so the six go by one pointer. */
#include <sys/sysctl.h>
#include <string.h>
#include "syscall.h"

int sysctl(const int *name, unsigned int namelen, void *oldp, size_t *oldlenp,
	const void *newp, size_t newlen)
{
	unsigned long args[6] = {
		(unsigned long)name, namelen, (unsigned long)oldp,
		(unsigned long)oldlenp, (unsigned long)newp, newlen,
	};
	return syscall(SYS_sysctl, args);
}

/* {0, 3}: the kernel turns the name written as new data into its numeric name. */
int sysctlnametomib(const char *name, int *mibp, size_t *sizep)
{
	static const int name2oid[2] = { 0, 3 };
	size_t len = *sizep * sizeof(int);
	int r = sysctl(name2oid, 2, mibp, &len, name, strlen(name));
	if (r == 0) *sizep = len / sizeof(int);
	return r;
}

int sysctlbyname(const char *name, void *oldp, size_t *oldlenp,
	const void *newp, size_t newlen)
{
	int mib[CTL_MAXNAME];
	size_t miblen = CTL_MAXNAME;
	if (sysctlnametomib(name, mib, &miblen) < 0) return -1;
	return sysctl(mib, miblen, oldp, oldlenp, newp, newlen);
}
