#ifndef _PPM_H_
#define _PPM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <malloc.h>
#include <assert.h>

typedef struct
{
	uint8_t *r;
	uint8_t *g;
	uint8_t *b;
	
	uint16_t w;
	uint16_t h;
	uint16_t max;
} ppm_t;


ppm_t* ppm_read(const char *fname);
void ppm_write(const char *fname, const ppm_t *ppm);
void ppm_free(ppm_t *ppm);
ppm_t* ppm_create(uint16_t w, uint16_t h, uint16_t max);
uint8_t* ppm_alloc_aligned(uint16_t w, uint16_t h);
float* ppm_alloc_aligned_f(uint16_t w, uint16_t h);


#endif // _PPM_H_