#ifndef _COMPINTRIN_H_
#define _COMPINTRIN_H_

#include <emmintrin.h>

__m128i _mm_scale_epu8 (__m128i x, __m128i y);
__m128i _mm_div255_epu16 (__m128i x);

#endif // _COMPINTRIN_H_