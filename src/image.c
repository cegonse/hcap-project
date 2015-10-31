#include "image.h"

ppm_t* img_brighten(ppm_t* src, float k, double* cycles)
{
	if (k < 0) return src;
	int i = 0, j = 0;
	uint16_t tempR, tempG, tempB;
	
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

	return src;
}


ppm_t* img_bw(ppm_t* src, double* cycles)
{
	int i = 0, j = 0;
	uint8_t h = 0;
	float corr = 1.0f / 3.0f;
	
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
	
	return src;
}


ppm_t* img_sharpen(ppm_t* src, float k, double* cycles)
{
	int i = 0, j = 0;
	float temp = 0.0f;
	float PSF[9] = {-k/8.0, -k/8.0, -k/8.0, -k/8.0, k+1.0, -k/8.0, -k/8.0, -k/8.0, -k/8.0};
	
	// Initialize data
	uint8_t* convR = NULL;
	uint8_t* convG = NULL;
	uint8_t* convB = NULL;
	
	convR = ppm_alloc_aligned(src->w, src->h);
	convG = ppm_alloc_aligned(src->w, src->h);
	convB = ppm_alloc_aligned(src->w, src->h);
	
	memcpy(convR, src->r, src->w * src->h * sizeof(uint8_t));
	memcpy(convG, src->g, src->w * src->h * sizeof(uint8_t));
	memcpy(convB, src->b, src->w * src->h * sizeof(uint8_t));
	
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
        for (i = 1; i < src->h - 1; i++)
        {
			temp = 0.0f;
            temp += (PSF[0] * src->r[i-1+(j-1)*src->h]);
            temp += (PSF[1] * src->r[i-1+(j)*src->h]);
            temp += (PSF[2] * src->r[i-1+(j+1)*src->h]);
            temp += (PSF[3] * src->r[i+(j-1)*src->h]);
            temp += (PSF[4] * src->r[i+(j)*src->h]);
            temp += (PSF[5] * src->r[i+(j+1)*src->h]);
            temp += (PSF[6] * src->r[i+1+(j-1)*src->h]);
            temp += (PSF[7] * src->r[i+1+(j)*src->h]);
            temp += (PSF[8] * src->r[i+1+(j+1)*src->h]);
			
			if (temp < 0) temp = 0;
			if (temp > 255) temp = 255;
			
			convR[i + src->h*j] = (uint8_t)temp;

            temp = 0.0f;
            temp += (PSF[0] * src->g[i-1+(j-1)*src->h]);
            temp += (PSF[1] * src->g[i-1+(j)*src->h]);
            temp += (PSF[2] * src->g[i-1+(j+1)*src->h]);
            temp += (PSF[3] * src->g[i+(j-1)*src->h]);
            temp += (PSF[4] * src->g[i+(j)*src->h]);
            temp += (PSF[5] * src->g[i+(j+1)*src->h]);
            temp += (PSF[6] * src->g[i+1+(j-1)*src->h]);
            temp += (PSF[7] * src->g[i+1+(j)*src->h]);
            temp += (PSF[8] * src->g[i+1+(j+1)*src->h]);
			
			if (temp < 0) temp = 0;
			if (temp > 255) temp = 255;
			
			convG[i + src->h*j] = (uint8_t)temp;

            temp=0;
            temp += (PSF[0] * src->b[i-1+(j-1)*src->h]);
            temp += (PSF[1] * src->b[i-1+(j)*src->h]);
            temp += (PSF[2] * src->b[i-1+(j+1)*src->h]);
            temp += (PSF[3] * src->b[i+(j-1)*src->h]);
            temp += (PSF[4] * src->b[i+(j)*src->h]);
            temp += (PSF[5] * src->b[i+(j+1)*src->h]);
            temp += (PSF[6] * src->b[i+1+(j-1)*src->h]);
            temp += (PSF[7] * src->b[i+1+(j)*src->h]);
            temp += (PSF[8] * src->b[i+1+(j+1)*src->h]);
			
			if (temp < 0) temp = 0;
			if (temp > 255) temp = 255;
			
			convB[i + src->h*j] = (uint8_t)temp;
        }
    }
	
	// Copy result to the original matrix
	memcpy(src->r, convR, src->w * src->h * sizeof(uint8_t));
	memcpy(src->g, convG, src->w * src->h * sizeof(uint8_t));
	memcpy(src->b, convB, src->w * src->h * sizeof(uint8_t));
	
	free(convR);
	free(convG);
	free(convB);
	
	return src;
}

