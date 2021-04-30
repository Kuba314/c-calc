

#ifndef __ERROR_H__
#define __ERROR_H__

#define ERR_ALLOC 1
#define ERR_SYNTAX 2
#define ERR_REDUCE 3
#define ERR_MAXSYM 4
#define ERR_MAXARGS 5
#define ERR_NOSYM 6
#define ERR_NOFUNC 7

#define ESHIFT 3UL
#define EMASK (~(~0UL << ESHIFT))

#define ALLOC_ERROR (ERR_ALLOC)
#define SYNTAX_ERROR(ix) (((ix) << ESHIFT) | ERR_SYNTAX)
#define REDUCE_ERROR (ERR_REDUCE)
#define MAXSYM_ERROR(size) ((size) << ESHIFT | ERR_MAXSYM)
#define MAXARGS_ERROR(size) ((size) << ESHIFT | ERR_MAXARGS)
#define NOSYM_ERROR (ERR_NOSYM)
#define NOFUNC_ERROR (ERR_NOFUNC)

void math_perror(int err, const char *err_msg);

void fatal(const char *fmt, ...);

#endif // __ERROR_H__
