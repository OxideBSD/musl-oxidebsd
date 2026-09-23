/*
 * A deliberately minimal `<sys/cdefs.h>` compat shim -- musl doesn't ship one at all (a real
 * BSD-ism, same category as `sys/queue.h` alongside it), and real FreeBSD's own version isn't a
 * clean standalone drop-in: it `#include`s further FreeBSD-internal headers (`sys/_decls.h`, ...)
 * that don't exist outside FreeBSD's own source tree. Rather than chase that whole cascade, this
 * defines just the handful of macros real BSD-heritage userland code (starting with `sys/queue.h`
 * itself, needed by OpenVi's `cl/cl_*.c` -- see CLAUDE.md's ncurses/nano/nvi section) actually
 * reaches for. Extend this file as real gaps surface, the same way every other musl patch here
 * does -- don't reach for FreeBSD's full version instead.
 */
#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

#include <stddef.h>

#if defined(__cplusplus)
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#define __containerof(x, s, m) ((s *)(void *)((char *)(x) - offsetof(s, m)))

#if defined(__GNUC__) || defined(__clang__)
#define __predict_true(exp) __builtin_expect(((exp) != 0), 1)
#define __predict_false(exp) __builtin_expect(((exp) != 0), 0)
#else
#define __predict_true(exp) (exp)
#define __predict_false(exp) (exp)
#endif

#endif /* !_SYS_CDEFS_H_ */
