.text
.global __clone
.hidden __clone
.type   __clone,@function
__clone:
	/* OxideBSD: hand-written asm bypasses bits/syscall.h.in's remap table -- must not use a raw
	 * Linux syscall number directly (same bug class already fixed once for vfork.s). SYS_CLONE
	 * is reserved at 555 (see docs/MISSING_POSIX_SYSCALLS.md), not real Linux's 56 -- no kernel
	 * handler exists yet, so this still cleanly ENOSYS's until real threading is implemented. */
	mov $555,%eax
	mov %rdi,%r11
	mov %rdx,%rdi
	mov %r8,%rdx
	mov %r9,%r8
	mov 8(%rsp),%r10
	mov %r11,%r9
	and $-16,%rsi
	sub $8,%rsi
	mov %rcx,(%rsi)
	syscall
	/* OxideBSD: same carry-flag->negative-errno fixup every ordinary syscall gets from
	 * syscall_arch.h's __syscallN wrappers (see that file's own doc comment) -- this hand-written
	 * stub bypasses those wrappers entirely, so without this the raw *positive* errno this kernel
	 * returns on failure (CF=1) reads to the C caller as a small, valid-looking child pid instead
	 * of a negative error. Found live: musl's own posix_spawn (used by the real on-target
	 * Clang/LLVM port's driver to launch `ld.lld`) issues clone(CLONE_VM|CLONE_VFORK|SIGCHLD, ...),
	 * a flag combination this kernel's own do_clone rejects with EINVAL(22) -- misread as "child
	 * pid 22" here, so posix_spawn proceeded to waitpid(22, ...), which correctly failed ECHILD
	 * (pid 22 was never created) instead of posix_spawn itself ever seeing the real clone failure.
	 * Same bug class already fixed once for vfork.s/__unmapself.s (hardcoded real Linux syscall
	 * numbers) -- this is the ABI-convention half of that same "hand-written asm bypasses the
	 * normal fixup" risk, not the number-remap half. */
	jnc 2f
	neg %eax
2:	test %eax,%eax
	jnz 1f
	xor %ebp,%ebp
	pop %rdi
	call *%r9
	mov %eax,%edi
	/* thread fn returned normally: call real exit(status) via this ABI's own SYS_EXIT=1 (was
	 * raw Linux's 60 -- same bypass-the-remap-table bug, fixed the same way). */
	xor %eax,%eax
	mov $1,%al
	syscall
	hlt
1:	ret
