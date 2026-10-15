.global vfork
.type vfork,@function
vfork:
	pop %rdx
	/* OxideBSD: hand-written asm bypasses bits/syscall.h.in's remap table, so the number is
	 * spelled out: SYS_vfork is 586, OxideBSD's own (the vfork kernel module). The child borrows
	 * this address space and returns here on this stack, so the return address is kept in a
	 * register, not on the stack the child may overwrite. */
	mov $586,%eax
	syscall
	/* OxideBSD: a failed call sets the carry flag with a positive errno; __syscall_ret wants it
	 * negative (the conversion syscall_arch.h does for C callers, as in clone.s). */
	jnc 1f
	neg %rax
1:	push %rdx
	mov %rax,%rdi
	.hidden __syscall_ret
	jmp __syscall_ret
