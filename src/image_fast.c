#include "image.h"


ppm_t* img_fast_bw(ppm_t* src, double* cycles)
{
	/*int i = 0, j = 0;
	float corr = 1.0f / 3.0f;
	__m128 vcorr = _mm_load1_ps(&corr);
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i += 16)
		{
			
		}
	}
	*/
	return src;
}


ppm_t* img_fast_brighten(ppm_t* src, float k, double* cycles)
{
	/*if (k < 0) return src;
	
	int i = 0, j = 0;
	float m = 1.0f;
	__m128 vk = _mm_load1_ps(&k);
	__m128 max = _mm_load1_ps(&m);
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i += 4)
		{
			__m128 tempR = _mm_load_ps(src->r + i + src->h*j);	
			__m128 tempG = _mm_load_ps(src->g + i + src->h*j);
			__m128 tempB = _mm_load_ps(src->b + i + src->h*j);
			
			// Brightening
			tempR = _mm_mul_ps(tempR, vk);
			tempG = _mm_mul_ps(tempG, vk);
			tempB = _mm_mul_ps(tempB, vk);
			
			// Bounding
			tempR =_mm_min_ps(tempR, max);
			tempG =_mm_min_ps(tempG, max);
			tempB =_mm_min_ps(tempB, max);
			
			_mm_store_ps(src->r + i + src->h*j, tempR);
			_mm_store_ps(src->g + i + src->h*j, tempG);
			_mm_store_ps(src->b + i + src->h*j, tempB);
		}
	}
*/
	return src;
}


ppm_t* img_fast_sharpen(ppm_t* src, float k, double* cycles)
{
	/*int i = 0, j = 0;
	float temp = 0.0f;
	float PSF[12] = {-k/8.0, -k/8.0, -k/8.0, -k/8.0, k+1.0, -k/8.0, -k/8.0, -k/8.0, -k/8.0, 0.0, 0.0, 0.0};
	float corr = 1.0f / (float)src->max;
	
	__m128 vfilter0 = _mm_load_ps(PSF);
	__m128 vfilter1 = _mm_load_ps(PSF + 4);
	__m128 vfilter2 = _mm_load_ps(PSF + 8);
	__m128 vcorr = _mm_load1_ps(&corr);
	
	// Initialize data
	float* convR = NULL;
	float* convG = NULL;
	float* convB = NULL;
	
	convR = ppm_alloc_aligned(src->w, src->h);
	convG = ppm_alloc_aligned(src->w, src->h);
	convB = ppm_alloc_aligned(src->w, src->h);
	
	memcpy(convR, src->r, src->w * src->h * sizeof(float));
	memcpy(convG, src->g, src->w * src->h * sizeof(float));
	memcpy(convB, src->b, src->w * src->h * sizeof(float));
	
	if (convR == NULL || convG == NULL || convB == NULL)
	{
		#ifdef DEBUG
		printf("Error allocating memory\n");
		#endif
		return NULL;
	}
	
	// Skip first and last row, no neighbors to convolve with
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
    }
	
	// Copy result to the original matrix
	memcpy(src->r, convR, src->w * src->h * sizeof(float));
	memcpy(src->g, convG, src->w * src->h * sizeof(float));
	memcpy(src->b, convB, src->w * src->h * sizeof(float));
	
	free(convR);
	free(convG);
	free(convB);
	*/
	return src;
}