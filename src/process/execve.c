#include <unistd.h>
#include <string.h>
#include "syscall.h"

/* OxideBSD patch: real execve(path, argv, envp) passes NUL-terminated char** arrays in
 * RDI/RSI/RDX. OxideBSD's own SYS_execve (src/process.rs's do_execve in the OxideBSD tree) instead
 * expects (path_ptr, path_len, argv_ptr, envp_ptr): path_ptr/path_len name the path directly (no
 * NUL-terminator requirement), and argv_ptr/envp_ptr each point at a sequence of {ptr, len} pairs
 * (16 bytes each, matching src/process.rs's RawArgvEntry exactly), terminated by a {0, 0} entry --
 * see CLAUDE.md's BusyBox section for the full story of why this call needed patching at all.
 *
 * argv_ptr now carries the *complete* argv[] array, starting at argv[0] -- real argv[0] (which a
 * caller can set to anything, not necessarily equal to path) is no longer dropped. This matches
 * do_execve's own current wire-format contract in the OxideBSD tree (see RawArgvEntry's own doc
 * comment there): a non-empty argv_ptr supplies argv[0] onward directly; passing no argv at all
 * (argv == NULL or argv[0] == NULL) still falls back to argv_ptr == 0, which the kernel expands
 * into a synthesized single-element argv = [path].
 */

struct raw_argv_entry {
	unsigned long ptr;
	unsigned long len;
};

/* Mirrors src/process/lifecycle.rs's own MAX_PTR_LEN_ENTRIES (256, raised from an original 32 for
 * the Clang/LLVM port -- a real clang -cc1 invocation already carries ~25-50 tokens for even a
 * trivial hello.c) -- the kernel silently stops reading past that many entries regardless, so
 * building more here would just be wasted stack space. This constant going stale relative to the
 * kernel's own is a real, previously-live bug: a real clang driver -> cc1 subprocess exec with
 * more than 32 argv entries got silently truncated mid-flag right here (this array, not the
 * kernel side, was the actual bottleneck), producing bogus cc1 parse errors like "argument to
 * '-internal-isystem' is missing" whenever enough preceding flags (e.g. -dumpdir/-static-define)
 * pushed a later flag's *value* past index 31 -- found live via a real on-target `clang -static -o
 * /hello.elf /hello.c` invocation, which forks a real `clang -cc1` subprocess (unlike a bare `-c`
 * compile, which clang runs in-process, never touching this wrapper's own argv-copy loop at all). */
#define MAX_EXECVE_ENTRIES 256

int execve(const char *path, char *const argv[], char *const envp[])
{
	struct raw_argv_entry argv_entries[MAX_EXECVE_ENTRIES + 1];
	struct raw_argv_entry envp_entries[MAX_EXECVE_ENTRIES + 1];
	int i;

	for (i = 0; argv && argv[i] && i < MAX_EXECVE_ENTRIES; i++) {
		argv_entries[i].ptr = (unsigned long)argv[i];
		argv_entries[i].len = strlen(argv[i]);
	}
	argv_entries[i].ptr = 0;
	argv_entries[i].len = 0;

	for (i = 0; envp && envp[i] && i < MAX_EXECVE_ENTRIES; i++) {
		envp_entries[i].ptr = (unsigned long)envp[i];
		envp_entries[i].len = strlen(envp[i]);
	}
	envp_entries[i].ptr = 0;
	envp_entries[i].len = 0;

	long ret = __syscall4(SYS_execve, (long)path, (long)strlen(path),
		(long)argv_entries, (long)envp_entries);
	return __syscall_ret(ret);
}
