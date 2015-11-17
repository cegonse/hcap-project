#include "image.h"


ppm_t* img_fast_bw(ppm_t* src, uint64_t* cycles)
{
	int i = 0, j = 0;
	
	uint8_t* r = src->r;
	uint8_t* g = src->g;
	uint8_t* b = src->b;
	int ni = 0;
	
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5;
	
	uint64_t t0 = readTSC();
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i += 16)
		{
			ni += 16;
			
			// Load 16 RGB pixels
			xmm0 = _mm_load_si128((__m128i*)(r + ni));
			xmm1 = _mm_load_si128((__m128i*)(g + ni));
			xmm2 = _mm_load_si128((__m128i*)(b + ni));
			
			// Average by pairs (RG, GB), then average pairs
			xmm3 = _mm_avg_epu8(xmm0, xmm1);
			xmm4 = _mm_avg_epu8(xmm1, xmm2);
			xmm5 = _mm_avg_epu8(xmm0, xmm1);
			
			// Store back
			_mm_store_si128((__m128i*)(r + ni), xmm5);
			_mm_store_si128((__m128i*)(g + ni), xmm5);
			_mm_store_si128((__m128i*)(b + ni), xmm5);
		}
	}
	
	if (cycles != NULL) *cycles = cyclesElapsed(readTSC(), t0);
	
	return src;
}


ppm_t* img_fast_brighten(ppm_t* src, float k, uint64_t* cycles)
{
	if (k < 0) return src;
	
	int i = 0, j = 0;
	
	uint8_t* r = src->r;
	uint8_t* g = src->g;
	uint8_t* b = src->b;
	int ni = 0;
	
	__m128i xmm0, xmm1, xmm2, xmm4, xmm5, xmm6;
	__m128i xmm3 = _mm_set1_epi8((int)ceil(k));
	
	uint64_t t0 = readTSC();
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i += 16)
		{
			ni += 16;
			
			// Load 16 RGB pixels
			xmm0 = _mm_load_si128((__m128i*)(r + ni));
			xmm1 = _mm_load_si128((__m128i*)(g + ni));
			xmm2 = _mm_load_si128((__m128i*)(b + ni));
			
			// Multiply by the coefficient vector
			xmm4 = _mm_scale_epu8(xmm0, xmm3);
			xmm5 = _mm_scale_epu8(xmm1, xmm3);
			xmm6 = _mm_scale_epu8(xmm2, xmm3);
			
			// Store back
			_mm_store_si128((__m128i*)(r + ni), xmm4);
			_mm_store_si128((__m128i*)(g + ni), xmm5);
			_mm_store_si128((__m128i*)(b + ni), xmm6);
		}
	}
	
	if (cycles != NULL) *cycles = cyclesElapsed(readTSC(), t0);
	
	return src;
}


ppm_t* img_fast_sharpen(ppm_t* src, ppm_t* dst, float k, uint64_t* cycles)
{
	int i = 0, j = 0;
	float temp = 0.0f;
	float PSF[12] __attribute__((aligned(16)))
		= {-k/8.0f, -k/8.0f, -k/8.0f, 0.0f, -k/8.0f, k+1.0f, -k/8.0f, 0.0f, -k/8.0f, -k/8.0f, -k/8.0f, 0.0f};
	float tempr[4] __attribute__((aligned(16)))
		= { 0.0f };
	int mask[4] __attribute__((aligned(16)))
		= { 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000 };
	
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9;
	__m128i xmm11;
	
	memcpy(dst->r, src->r, src->h*src->w*sizeof(uint8_t));
	memcpy(dst->g, src->g, src->h*src->w*sizeof(uint8_t));
	memcpy(dst->b, src->b, src->h*src->w*sizeof(uint8_t));
	
	uint64_t t0 = readTSC();
	_mm_empty();
	
	xmm0 = _mm_load_ps(PSF);
	xmm1 = _mm_load_ps(PSF + 4);
	xmm2 = _mm_load_ps(PSF + 8);
	
	// Skip first and last row, no neighbors to convolve with
    for (j = 1; j < src->w - 1; j++)
    {
        // Skip first and last column, no neighbors to convolve with
        for (i = 1; i < src->h - 1; i++)
        {
            temp = 0.0f;
			
			// Load 9 values
			//xmm3 = _mm_cvtpu8_ps(*(__m64*)&src->r[i + (j-1)*src->h - 1]);
			//xmm4 = _mm_cvtpu8_ps(*(__m64*)&src->r[i + (j)*src->h - 1]);
			//xmm5 = _mm_cvtpu8_ps(*(__m64*)&src->r[i + (j+1)*src->h - 1]);
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->r[i + (j-1)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm3 = _mm_cvtepi32_ps(xmm11);
			
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->r[i + (j)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm4 = _mm_cvtepi32_ps(xmm11);
			
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->r[i + (j+1)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm5 = _mm_cvtepi32_ps(xmm11);
			
			// Multiply
			xmm6 = _mm_mul_ps(xmm0, xmm3);
			xmm7 = _mm_mul_ps(xmm1, xmm4);
			xmm8 = _mm_mul_ps(xmm2, xmm5);
			
			// Add
			xmm9 = _mm_add_ps(xmm6, _mm_add_ps(xmm7, xmm8));
			
			_mm_store_ps(tempr, xmm9);
			temp = tempr[0] + tempr[1] + tempr[2];
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			dst->r[i + src->h*j] = (uint8_t)temp;
			temp = 0.0f;
			
            // Load 9 values
			//xmm3 = _mm_cvtpu8_ps(*(__m64*)&src->g[i + (j-1)*src->h - 1]);
			//xmm4 = _mm_cvtpu8_ps(*(__m64*)&src->g[i + (j)*src->h - 1]);
			//xmm5 = _mm_cvtpu8_ps(*(__m64*)&src->g[i + (j+1)*src->h - 1]);
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->g[i + (j-1)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm3 = _mm_cvtepi32_ps(xmm11);
			
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->g[i + (j)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm4 = _mm_cvtepi32_ps(xmm11);
			
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->g[i + (j+1)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm5 = _mm_cvtepi32_ps(xmm11);
			
			// Multiply
			xmm6 = _mm_mul_ps(xmm0, xmm3);
			xmm7 = _mm_mul_ps(xmm1, xmm4);
			xmm8 = _mm_mul_ps(xmm2, xmm5);
			
			// Add
			xmm9 = _mm_add_ps(xmm6, _mm_add_ps(xmm7, xmm8));
			
			_mm_store_ps(tempr, xmm9);
			temp = tempr[0] + tempr[1] + tempr[2];
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			dst->g[i + src->h*j] = (uint8_t)temp;
			temp = 0.0f;
			
			// Load 9 values
			//xmm3 = _mm_cvtpu8_ps(*(__m64*)&src->b[i + (j-1)*src->h - 1]);
			//xmm4 = _mm_cvtpu8_ps(*(__m64*)&src->b[i + (j)*src->h - 1]);
			//xmm5 = _mm_cvtpu8_ps(*(__m64*)&src->b[i + (j+1)*src->h - 1]);
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->b[i + (j-1)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm3 = _mm_cvtepi32_ps(xmm11);
			
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->b[i + (j)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm4 = _mm_cvtepi32_ps(xmm11);
			
			xmm11 = _mm_cvtsi32_si128(*(const int*)&src->b[i + (j+1)*src->h - 1]);
			xmm11 = _mm_unpacklo_epi8(xmm11, _mm_setzero_si128());
			xmm11 = _mm_unpacklo_epi16(xmm11, _mm_setzero_si128());
			xmm5 = _mm_cvtepi32_ps(xmm11);
			
			// Multiply
			xmm6 = _mm_mul_ps(xmm0, xmm3);
			xmm7 = _mm_mul_ps(xmm1, xmm4);
			xmm8 = _mm_mul_ps(xmm2, xmm5);
			
			// Add
			xmm9 = _mm_add_ps(xmm6, _mm_add_ps(xmm7, xmm8));
			
			_mm_store_ps(tempr, xmm9);
			temp = tempr[0] + tempr[1] + tempr[2];
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			dst->b[i + src->h*j] = (uint8_t)temp;
        }
    }
	
	_mm_empty();
	if (cycles != NULL) *cycles = cyclesElapsed(readTSC(), t0);
	
	return dst;
}


ppm_t* img_fast_sharpen_copy(ppm_t* src, ppm_t* dst, float k, uint64_t* cycles)
{
	int i = 0, j = 0;
	float temp = 0.0f;
	float PSF[12] __attribute__((aligned(16)))
		= {-k/8.0f, -k/8.0f, -k/8.0f, 0.0f, -k/8.0f, k+1.0f, -k/8.0f, 0.0f, -k/8.0f, -k/8.0f, -k/8.0f, 0.0f};
	float tempr[4] __attribute__((aligned(16)))
		= { 0.0f };
	
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9;
	
	// Initialize data
	float* copyR = ppm_alloc_aligned_f(src->w, src->h);
	float* copyG = ppm_alloc_aligned_f(src->w, src->h);
	float* copyB = ppm_alloc_aligned_f(src->w, src->h);
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i++)
		{
			copyR[i + src->h*j] = (float)src->r[i + src->h*j];
			copyG[i + src->h*j] = (float)src->g[i + src->h*j];
			copyB[i + src->h*j] = (float)src->b[i + src->h*j];
			
			dst->r[i + src->h*j] = src->r[i + src->h*j];
			dst->g[i + src->h*j] = src->g[i + src->h*j];
			dst->b[i + src->h*j] = src->b[i + src->h*j];
		}
	}
	
	uint64_t t0 = readTSC();
	_mm_empty();
	
	xmm0 = _mm_load_ps(PSF);
	xmm1 = _mm_load_ps(PSF + 4);
	xmm2 = _mm_load_ps(PSF + 8);
	
	// Skip first and last row, no neighbors to convolve with
    for (j = 1; j < src->w - 1; j++)
    {
        // Skip first and last column, no neighbors to convolve with
        for (i = 1; i < src->h - 1; i++)
        {
            temp = 0.0f;

			// Load 9 values
			if ((i-1) % 4 == 0)
			{
				xmm3 = _mm_load_ps(&copyR[i + (j-1)*src->h] - 1);
				xmm4 = _mm_load_ps(&copyR[i + (j)*src->h] - 1);
				xmm5 = _mm_load_ps(&copyR[i + (j+1)*src->h] - 1);
			}
			else
			{
				xmm3 = _mm_loadu_ps(&copyR[i + (j-1)*src->h] - 1);
				xmm4 = _mm_loadu_ps(&copyR[i + (j)*src->h] - 1);
				xmm5 = _mm_loadu_ps(&copyR[i + (j+1)*src->h] - 1);
			}
			
			// Multiply
			xmm6 = _mm_mul_ps(xmm0, xmm3);
			xmm7 = _mm_mul_ps(xmm1, xmm4);
			xmm8 = _mm_mul_ps(xmm2, xmm5);
			
			// Add
			xmm9 = _mm_add_ps(xmm6, _mm_add_ps(xmm7, xmm8));
			_mm_store_ps(tempr, xmm9);
			
			temp = tempr[0] + tempr[1] + tempr[2];
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			dst->r[i + src->h*j] = (uint8_t)temp;
			temp = 0.0f;
			
            // Load 9 values
			if ((i-1) % 4 == 0)
			{
				xmm3 = _mm_load_ps(&copyG[i + (j-1)*src->h] - 1);
				xmm4 = _mm_load_ps(&copyG[i + (j)*src->h] - 1);
				xmm5 = _mm_load_ps(&copyG[i + (j+1)*src->h] - 1);
			}
			else
			{
				xmm3 = _mm_loadu_ps(&copyG[i + (j-1)*src->h] - 1);
				xmm4 = _mm_loadu_ps(&copyG[i + (j)*src->h] - 1);
				xmm5 = _mm_loadu_ps(&copyG[i + (j+1)*src->h] - 1);
			}
			
			// Multiply
			xmm6 = _mm_mul_ps(xmm0, xmm3);
			xmm7 = _mm_mul_ps(xmm1, xmm4);
			xmm8 = _mm_mul_ps(xmm2, xmm5);
			
			// Add
			xmm9 = _mm_add_ps(xmm6, _mm_add_ps(xmm7, xmm8));
			_mm_store_ps(tempr, xmm9);
			
			temp = tempr[0] + tempr[1] + tempr[2];
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			dst->g[i + src->h*j] = (uint8_t)temp;
			temp = 0.0f;
			
			// Load 9 values
			if ((i-1) % 4 == 0)
			{
				xmm3 = _mm_load_ps(&copyB[i + (j-1)*src->h] - 1);
				xmm4 = _mm_load_ps(&copyB[i + (j)*src->h] - 1);
				xmm5 = _mm_load_ps(&copyB[i + (j+1)*src->h] - 1);
			}
			else
			{
				xmm3 = _mm_loadu_ps(&copyB[i + (j-1)*src->h] - 1);
				xmm4 = _mm_loadu_ps(&copyB[i + (j)*src->h] - 1);
				xmm5 = _mm_loadu_ps(&copyB[i + (j+1)*src->h] - 1);
			}
			
			// Multiply
			xmm6 = _mm_mul_ps(xmm0, xmm3);
			xmm7 = _mm_mul_ps(xmm1, xmm4);
			xmm8 = _mm_mul_ps(xmm2, xmm5);
			
			// Add
			xmm9 = _mm_add_ps(xmm6, _mm_add_ps(xmm7, xmm8));
			_mm_store_ps(tempr, xmm9);
			
			temp = tempr[0] + tempr[1] + tempr[2];
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			dst->b[i + src->h*j] = (uint8_t)temp;
        }
    }
	
	_mm_empty();
	
	if (cycles != NULL) *cycles = cyclesElapsed(readTSC(), t0);

	#ifdef __INTEL_COMPILER
	_mm_free(copyR);
	#else
	free(copyR);
	#endif
	
	#ifdef __INTEL_COMPILER
	_mm_free(copyG);
	#else
	free(copyG);
	#endif
	
	#ifdef __INTEL_COMPILER
	_mm_free(copyB);
	#else
	free(copyB);
	#endif
	
	return dst;
}
