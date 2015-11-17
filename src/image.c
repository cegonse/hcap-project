#include "image.h"

ppm_t* img_brighten(ppm_t* src, float k, uint64_t* cycles)
{
	if (k < 0) return src;
	
	int i = 0, j = 0;
	uint16_t tempR, tempG, tempB;
	
	uint64_t t0 = readTSC();
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i++)
		{
			tempR = src->r[i + src->h*j] * k;
			tempG = src->g[i + src->h*j] * k;
			tempB = src->b[i + src->h*j] * k;
			
			if (tempR > 255) tempR = 255;
			if (tempG > 255) tempG = 255;
			if (tempB > 255) tempB = 255;
			
			src->r[i + src->h*j] = tempR;
			src->g[i + src->h*j] = tempG;
			src->b[i + src->h*j] = tempB;
		}
	}
	
	if (cycles != NULL) *cycles = cyclesElapsed(readTSC(), t0);
	
	return src;
}


ppm_t* img_bw(ppm_t* src, uint64_t* cycles)
{
	int i = 0, j = 0;
	uint8_t h = 0;
	float corr = 1.0f / 3.0f;
	
	uint64_t t0 = readTSC();
	
	for (j = 0; j < src->w; j++)
	{
		for (i = 0; i < src->h; i++)
		{
			h = (src->r[i + src->h*j] + src->g[i + src->h*j] + src->b[i + src->h*j]) * corr;
			
			src->r[i + src->h*j] = h;
			src->g[i + src->h*j] = h;
			src->b[i + src->h*j] = h;
		}
	}
	
	if (cycles != NULL) *cycles = cyclesElapsed(readTSC(), t0);
	
	return src;
}


ppm_t* img_sharpen(ppm_t* src, ppm_t* dst, float k, uint64_t* cycles)
{
	int i = 0, j = 0;
	float temp = 0.0f;
	float PSF[9] = {-k/8.0, -k/8.0, -k/8.0, -k/8.0, k+1.0, -k/8.0, -k/8.0, -k/8.0, -k/8.0};
	
	memcpy(dst->r, src->r, src->w * src->h * sizeof(uint8_t));
	memcpy(dst->g, src->g, src->w * src->h * sizeof(uint8_t));
	memcpy(dst->b, src->b, src->w * src->h * sizeof(uint8_t));
	
	uint64_t t0 = readTSC();
	
	// Skip first and last row, no neighbors to convolve with
    for (j = 1; j < src->w - 1; j++)
    {
        // Skip first and last column, no neighbors to convolve with
        for (i = 1; i < src->h - 1; i++)
        {
			temp = 0.0f;
            temp += (PSF[0] * (float)src->r[i-1+(j-1)*src->h]);
            temp += (PSF[1] * (float)src->r[i-1+(j)*src->h]);
            temp += (PSF[2] * (float)src->r[i-1+(j+1)*src->h]);
            temp += (PSF[3] * (float)src->r[i+(j-1)*src->h]);
            temp += (PSF[4] * (float)src->r[i+(j)*src->h]);
            temp += (PSF[5] * (float)src->r[i+(j+1)*src->h]);
            temp += (PSF[6] * (float)src->r[i+1+(j-1)*src->h]);
            temp += (PSF[7] * (float)src->r[i+1+(j)*src->h]);
            temp += (PSF[8] * (float)src->r[i+1+(j+1)*src->h]);
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > 255.0f) temp = (float)src->max;
			
			dst->r[i + src->h*j] = (uint8_t)temp;

            temp = 0.0f;
            temp += (PSF[0] * (float)src->g[i-1+(j-1)*src->h]);
            temp += (PSF[1] * (float)src->g[i-1+(j)*src->h]);
            temp += (PSF[2] * (float)src->g[i-1+(j+1)*src->h]);
            temp += (PSF[3] * (float)src->g[i+(j-1)*src->h]);
            temp += (PSF[4] * (float)src->g[i+(j)*src->h]);
            temp += (PSF[5] * (float)src->g[i+(j+1)*src->h]);
            temp += (PSF[6] * (float)src->g[i+1+(j-1)*src->h]);
            temp += (PSF[7] * (float)src->g[i+1+(j)*src->h]);
            temp += (PSF[8] * (float)src->g[i+1+(j+1)*src->h]);
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > 255.0f) temp = (float)src->max;
			
			dst->g[i + src->h*j] = (uint8_t)temp;

            temp=0;
            temp += (PSF[0] * (float)src->b[i-1+(j-1)*src->h]);
            temp += (PSF[1] * (float)src->b[i-1+(j)*src->h]);
            temp += (PSF[2] * (float)src->b[i-1+(j+1)*src->h]);
            temp += (PSF[3] * (float)src->b[i+(j-1)*src->h]);
            temp += (PSF[4] * (float)src->b[i+(j)*src->h]);
            temp += (PSF[5] * (float)src->b[i+(j+1)*src->h]);
            temp += (PSF[6] * (float)src->b[i+1+(j-1)*src->h]);
            temp += (PSF[7] * (float)src->b[i+1+(j)*src->h]);
            temp += (PSF[8] * (float)src->b[i+1+(j+1)*src->h]);
			
			if (temp < 0.0f) temp = 0.0f;
			if (temp > 255.0f) temp = (float)src->max;
			
			dst->b[i + src->h*j] = (uint8_t)temp;
        }
    }
	
	if (cycles != NULL) *cycles = cyclesElapsed(readTSC(), t0);
	return dst;
}


uint64_t readTSC()
{
   unsigned hi, lo;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
   return ((uint64_t) lo) | (((uint64_t) hi) << 32);
}


uint64_t cyclesElapsed(uint64_t stopTS, uint64_t startTS)
{
   return (stopTS - startTS);
}

