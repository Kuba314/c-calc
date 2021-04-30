

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

typedef struct {
    long double real;
    long double imag;
} complex_t;

complex_t mk_complex(long double real, long double imag);

#endif // __COMPLEX_H__
