#define SYSCALL_NO_TLS 1
#include <elf.h>
#include <limits.h>
#include <sys/mman.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "pthread_impl.h"
#include "libc.h"
#include "atomic.h"
#include "syscall.h"

volatile int __thread_list_lock;

int __init_tp(void *p)
{
	pthread_t td = p;
	td->self = td;
	int r = __set_thread_area(TP_ADJ(p));
	if (r < 0) return -1;
	if (!r) libc.can_do_threads = 1;
	td->detach_state = DT_JOINABLE;
	td->tid = __syscall(SYS_set_tid_address, &__thread_list_lock);
	td->locale = &libc.global_locale;
	td->robust_list.head = &td->robust_list.head;
	td->sysinfo = __sysinfo;
	td->next = td->prev = td;
	return 0;
}

static struct builtin_tls {
	char c;
	struct pthread pt;
	void *space[16];
} builtin_tls[1];
#define MIN_TLS_ALIGN offsetof(struct builtin_tls, pt)

static struct tls_module main_tls;

void *__copy_tls(unsigned char *mem)
{
	pthread_t td;
	struct tls_module *p;
	size_t i;
	uintptr_t *dtv;

#ifdef TLS_ABOVE_TP
	dtv = (uintptr_t*)(mem + libc.tls_size) - (libc.tls_cnt + 1);

	mem += -((uintptr_t)mem + sizeof(struct pthread)) & (libc.tls_align-1);
	td = (pthread_t)mem;
	mem += sizeof(struct pthread);

	for (i=1, p=libc.tls_head; p; i++, p=p->next) {
		dtv[i] = (uintptr_t)(mem + p->offset) + DTP_OFFSET;
		memcpy(mem + p->offset, p->image, p->len);
	}
#else
	dtv = (uintptr_t *)mem;

	mem += libc.tls_size - sizeof(struct pthread);
	mem -= (uintptr_t)mem & (libc.tls_align-1);
	td = (pthread_t)mem;

	for (i=1, p=libc.tls_head; p; i++, p=p->next) {
		dtv[i] = (uintptr_t)(mem - p->offset) + DTP_OFFSET;
		memcpy(mem - p->offset, p->image, p->len);
	}
#endif
	dtv[0] = libc.tls_cnt;
	td->dtv = dtv;
	return td;
}

#if ULONG_MAX == 0xffffffff
typedef Elf32_Phdr Phdr;
#else
typedef Elf64_Phdr Phdr;
#endif

extern weak hidden const size_t _DYNAMIC[];

static void static_init_tls(size_t *aux)
{
	unsigned char *p;
	size_t n;
	Phdr *phdr, *tls_phdr=0;
	size_t base = 0;
	void *mem;

	for (p=(void *)aux[AT_PHDR],n=aux[AT_PHNUM]; n; n--,p+=aux[AT_PHENT]) {
		phdr = (void *)p;
		if (phdr->p_type == PT_PHDR)
			base = aux[AT_PHDR] - phdr->p_vaddr;
		if (phdr->p_type == PT_DYNAMIC && _DYNAMIC)
			base = (size_t)_DYNAMIC - phdr->p_vaddr;
		if (phdr->p_type == PT_TLS)
			tls_phdr = phdr;
		if (phdr->p_type == PT_GNU_STACK &&
		    phdr->p_memsz > __default_stacksize)
			__default_stacksize =
				phdr->p_memsz < DEFAULT_STACK_MAX ?
				phdr->p_memsz : DEFAULT_STACK_MAX;
	}

	if (tls_phdr) {
		main_tls.image = (void *)(base + tls_phdr->p_vaddr);
		main_tls.len = tls_phdr->p_filesz;
		main_tls.size = tls_phdr->p_memsz;
		main_tls.align = tls_phdr->p_align;
		libc.tls_cnt = 1;
		libc.tls_head = &main_tls;
	}

	main_tls.size += (-main_tls.size - (uintptr_t)main_tls.image)
		& (main_tls.align-1);
#ifdef TLS_ABOVE_TP
	main_tls.offset = GAP_ABOVE_TP;
	main_tls.offset += (-GAP_ABOVE_TP + (uintptr_t)main_tls.image)
		& (main_tls.align-1);
#else
	main_tls.offset = main_tls.size;
#endif
	if (main_tls.align < MIN_TLS_ALIGN) main_tls.align = MIN_TLS_ALIGN;

	libc.tls_align = main_tls.align;
	libc.tls_size = 2*sizeof(void *) + sizeof(struct pthread)
#ifdef TLS_ABOVE_TP
		+ main_tls.offset
#endif
		+ main_tls.size + main_tls.align
		+ MIN_TLS_ALIGN-1 & -MIN_TLS_ALIGN;

	if (libc.tls_size > sizeof builtin_tls) {
#ifndef SYS_mmap2
#define SYS_mmap2 SYS_mmap
#endif
		/* OxideBSD patch: this raw syscall bypassed the already-patched __mmap() wrapper
		 * (src/mman/mmap.c) entirely, still passing real Linux 6-arg mmap semantics (flags
		 * as its own argument, fd/off as two more) that this kernel's own 4-register ABI
		 * can't parse -- flags must ride in prot's own unused high bits, fd/off packed into
		 * one more register, matching __mmap()'s own packing exactly. Never caught before:
		 * every prior on-target static binary (tcc, BusyBox, clang, ...) had a small enough
		 * PT_TLS to stay under `sizeof builtin_tls` and never reach this path at all -- found
		 * live via a real ld.lld crash (992-byte PT_TLS, the first to cross that threshold),
		 * a real page fault writing to address -14 (-EFAULT) -- this mmap call failing
		 * exactly as the comment below already expected of a genuine failure, just never one
		 * caused by this port's own wire-format mismatch before. */
		uint64_t prot_wire = ((uint64_t)(uint32_t)(PROT_READ|PROT_WRITE) & 0xff)
			| ((uint64_t)(uint32_t)(MAP_ANONYMOUS|MAP_PRIVATE) << 8);
		uint64_t packed = (uint64_t)(uint32_t)(-1);
		mem = (void *)__syscall(
			SYS_mmap2,
			0, libc.tls_size, prot_wire, packed);
		/* -4095...-1 cast to void * will crash on dereference anyway,
		 * so don't bloat the init code checking for error codes and
		 * explicitly calling a_crash(). */
	} else {
		mem = builtin_tls;
	}

	/* Failure to initialize thread pointer is always fatal. */
	if (__init_tp(__copy_tls(mem)) < 0)
		a_crash();
}

weak_alias(static_init_tls, __init_tls);
