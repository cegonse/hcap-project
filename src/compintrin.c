#include "compintrin.h"


inline __m128i _mm_div255_epu16 (__m128i x)
{
	// Divide 8 16-bit uints by 255:
	// x := ((x + 1) + (x >> 8)) >> 8:
	return _mm_srli_epi16(_mm_adds_epu16(
		_mm_adds_epu16(x, _mm_set1_epi16(1)),
		_mm_srli_epi16(x, 8)), 8);
}


__m128i _mm_scale_epu8 (__m128i x, __m128i y)
{
	// Returns an "alpha blend" of x scaled by y/255;
	//   x := x * (y / 255)
	// Reorder: x := (x * y) / 255

	// Unpack x and y into 16-bit uints:
	__m128i xlo = _mm_unpacklo_epi8(x, _mm_setzero_si128());
	__m128i ylo = _mm_unpacklo_epi8(y, _mm_setzero_si128());
	__m128i xhi = _mm_unpackhi_epi8(x, _mm_setzero_si128());
	__m128i yhi = _mm_unpackhi_epi8(y, _mm_setzero_si128());

	// Multiply x with y, keeping the low 16 bits:
	xlo = _mm_mullo_epi16(xlo, ylo);
	xhi = _mm_mullo_epi16(xhi, yhi);

	// Repack the 16-bit uints to clamped 8-bit values:
	return _mm_packus_epi16(xlo, xhi);
}