#ifndef _CONVOLVE_H_
#define _CONVOLVE_H_

#include "ppm.h"
#include "compintrin.h"

ppm_t* img_brighten(ppm_t* src, float k, uint64_t* cycles);
ppm_t* img_sharpen(ppm_t* src, float k, uint64_t* cycles);
ppm_t* img_bw(ppm_t* src, uint64_t* cycles);

ppm_t* img_fast_bw(ppm_t* src, uint64_t* cycles);
ppm_t* img_fast_brighten(ppm_t* src, float k, uint64_t* cycles);
ppm_t* img_fast_sharpen(ppm_t* src, ppm_t* dst, float k, uint64_t* cycles);
ppm_t* img_fast_sharpen_copy(ppm_t* src, float k, uint64_t* cycles);

uint64_t readTSC();
uint64_t cyclesElapsed(uint64_t stopTS, uint64_t startTS);

#endif