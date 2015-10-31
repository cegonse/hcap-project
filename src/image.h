#ifndef _CONVOLVE_H_
#define _CONVOLVE_H_

#include "ppm.h"
#include <immintrin.h>
#include <emmintrin.h>

ppm_t* img_brighten(ppm_t* src, float k, double* cycles);
ppm_t* img_sharpen(ppm_t* src, float k, double* cycles);
ppm_t* img_bw(ppm_t* src, double* cycles);

ppm_t* img_fast_bw(ppm_t* src, double* cycles);
ppm_t* img_fast_brighten(ppm_t* src, float k, double* cycles);
ppm_t* img_fast_sharpen(ppm_t* src, float k, double* cycles);

#endif