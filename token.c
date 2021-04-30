#include "token.h"

complex_t mk_complex(long double real, long double imag) {
    return (complex_t) {.real = real, .imag = imag};
}
