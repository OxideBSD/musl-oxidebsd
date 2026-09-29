#define _GNU_SOURCE
#include <stdlib.h>
#include <sys/sysctl.h>

/* OxideBSD: the BSDs' way, from vm.loadavg (SYSCTL.md §9.2). */
int getloadavg(double *a, int n)
{
	int mib[2] = { CTL_VM, VM_LOADAVG };
	struct loadavg la;
	size_t len = sizeof la;
	if (n <= 0) return n ? -1 : 0;
	if (sysctl(mib, 2, &la, &len, 0, 0) < 0) return -1;
	if (n > 3) n = 3;
	for (int i=0; i<n; i++)
		a[i] = (double)la.ldavg[i] / la.fscale;
	return n;
}
