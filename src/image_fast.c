#include "image.h"


ppm_t* img_fast_bw(ppm_t* src, double* cycles)
{
	int i = 0, j = 0;
	
	uint8_t* r = src->r;
	uint8_t* g = src->g;
	uint8_t* b = src->b;
	int ni = 0;
	
	__m128i xmm0, xmm1, xmm2;
	
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
			xmm0 = _mm_avg_epu8(xmm0, xmm1);
			xmm1 = _mm_avg_epu8(xmm1, xmm2);
			xmm2 = _mm_avg_epu8(xmm0, xmm1);
			
			// Store back
			_mm_store_si128((__m128i*)(r + ni), xmm2);
			_mm_store_si128((__m128i*)(g + ni), xmm2);
			_mm_store_si128((__m128i*)(b + ni), xmm2);
		}
	}
	
	return src;
}


ppm_t* img_fast_brighten(ppm_t* src, float k, double* cycles)
{
	if (k < 0) return src;
	
	int i = 0, j = 0;
	
	uint8_t* r = src->r;
	uint8_t* g = src->g;
	uint8_t* b = src->b;
	int ni = 0;
	
	__m128i xmm0, xmm1, xmm2;
	__m128i xmm3 = _mm_set1_epi8((int)ceil(k));
	
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
			xmm0 = _mm_scale_epu8(xmm0, xmm3);
			xmm1 = _mm_scale_epu8(xmm1, xmm3);
			xmm2 = _mm_scale_epu8(xmm2, xmm3);
			
			// Store back
			_mm_store_si128((__m128i*)(r + ni), xmm0);
			_mm_store_si128((__m128i*)(g + ni), xmm1);
			_mm_store_si128((__m128i*)(b + ni), xmm2);
		}
	}
	
	return src;
}


ppm_t* img_fast_sharpen(ppm_t* src, float k, double* cycles)
{
	int i = 0, j = 0;
	float temp = 0.0f;
	float PSF[12] = {-k/8.0, -k/8.0, -k/8.0, -k/8.0, k+1.0, -k/8.0, -k/8.0, -k/8.0, -k/8.0, 0.0, 0.0, 0.0};
	
	__m128 xmm0 = _mm_load_ps(PSF);
	__m128 xmm1 = _mm_load_ps(PSF + 4);
	__m128 xmm2 = _mm_load_ps(PSF + 8);
	
	// Initialize data
	float* convR = NULL;
	float* convG = NULL;
	float* convB = NULL;
	
	convR = ppm_alloc_aligned_f(src->w, src->h);
	convG = ppm_alloc_aligned_f(src->w, src->h);
	convB = ppm_alloc_aligned_f(src->w, src->h);
	
	if (convR == NULL || convG == NULL || convB == NULL)
	{
		#ifdef DEBUG
		printf("Error allocating memory\n");
		#endif
		return NULL;
	}
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i++)
		{
			convR[i + src->h*j] = (float)src->r[i + src->h*j];
			convG[i + src->h*j] = (float)src->g[i + src->h*j];
			convB[i + src->h*j] = (float)src->b[i + src->h*j];
		}
	}
	
	/*// Skip first and last row, no neighbors to convolve with
    for (j = 1; j < src->w - 1; j++)
    {
        // Skip first and last column, no neighbors to convolve with
        for (i = 1; i < src->h - 1; i += 4)
        {
            temp = 0.0f;
            temp += (PSF[0] * src->r[i-1+(j-1)*src->h] * src->max);
            temp += (PSF[1] * src->r[i-1+(j)*src->h] * src->max);
            temp += (PSF[2] * src->r[i-1+(j+1)*src->h] * src->max);
            temp += (PSF[3] * src->r[i+(j-1)*src->h] * src->max);
            temp += (PSF[4] * src->r[i+(j)*src->h] * src->max);
            temp += (PSF[5] * src->r[i+(j+1)*src->h] * src->max);
            temp += (PSF[6] * src->r[i+1+(j-1)*src->h] * src->max);
            temp += (PSF[7] * src->r[i+1+(j)*src->h] * src->max);
            temp += (PSF[8] * src->r[i+1+(j+1)*src->h] * src->max);
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			convR[i + src->h*j] = temp * corr;

            temp = 0.0f;
            temp += (PSF[0] * src->g[i-1+(j-1)*src->h] * src->max);
            temp += (PSF[1] * src->g[i-1+(j)*src->h] * src->max);
            temp += (PSF[2] * src->g[i-1+(j+1)*src->h] * src->max);
            temp += (PSF[3] * src->g[i+(j-1)*src->h] * src->max);
            temp += (PSF[4] * src->g[i+(j)*src->h] * src->max);
            temp += (PSF[5] * src->g[i+(j+1)*src->h] * src->max);
            temp += (PSF[6] * src->g[i+1+(j-1)*src->h] * src->max);
            temp += (PSF[7] * src->g[i+1+(j)*src->h] * src->max);
            temp += (PSF[8] * src->g[i+1+(j+1)*src->h] * src->max);
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			convG[i + src->h*j] = temp * corr;

            temp=0;
            temp += (PSF[0] * src->b[i-1+(j-1)*src->h] * src->max);
            temp += (PSF[1] * src->b[i-1+(j)*src->h] * src->max);
            temp += (PSF[2] * src->b[i-1+(j+1)*src->h] * src->max);
            temp += (PSF[3] * src->b[i+(j-1)*src->h] * src->max);
            temp += (PSF[4] * src->b[i+(j)*src->h] * src->max);
            temp += (PSF[5] * src->b[i+(j+1)*src->h] * src->max);
            temp += (PSF[6] * src->b[i+1+(j-1)*src->h] * src->max);
            temp += (PSF[7] * src->b[i+1+(j)*src->h] * src->max);
            temp += (PSF[8] * src->b[i+1+(j+1)*src->h] * src->max);
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > (float)src->max) temp = (float)src->max;
			
			convB[i + src->h*j] = temp * corr;
        }
    }*/
	
	// Copy result to the original matrix
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i++)
		{
			src->r[i + src->h*j] = (uint8_t)src->r[i + src->h*j];
			src->g[i + src->h*j] = (uint8_t)src->g[i + src->h*j];
			src->b[i + src->h*j] = (uint8_t)src->b[i + src->h*j];
		}
	}
	
	free(convR);
	free(convG);
	free(convB);
	
	return src;
}
