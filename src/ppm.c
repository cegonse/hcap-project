#include "ppm.h"

ppm_t* ppm_read(const char *fname)
{
	FILE* fd = fopen(fname, "r");
	
	if (fd == NULL)
	{
		#ifdef DEBUG
		printf("Error opening file descriptor\n");
		#endif
		
		return NULL;
	}
	
	// Read header
	ppm_t* ppm = (ppm_t*)malloc(sizeof(ppm_t));
	
	ppm->h = 0;
	ppm->w = 0;
	ppm->max = 0;
	
	ppm->r = NULL;
	ppm->g = NULL;
	ppm->b = NULL;
	
	char hd[128];
	int fstart = 3;
	
	while (fstart > 0)
	{
		fgets(hd, 128, fd);
		
		// Ignore comments
		if (hd[0] != '#')
		{
			if (fstart == 3)
			{
				// Read magic number
				if (strcmp("P6\n", hd) != 0)
				{
					// Invalid PPM file
					return NULL;
				}
				
				#ifdef DEBUG
				printf("Valid PPM file\n");
				#endif
				fstart--;
			}
			else if (fstart == 2)
			{
				char* tk = strtok(hd, " ");
				ppm->w = atoi(tk);
				
				tk = strtok(NULL, "\0");
				ppm->h = atoi(tk);
				
				#ifdef DEBUG
				printf("- Width: %d\n", ppm->w);
				printf("- Height: %d\n", ppm->h);
				#endif
				
				ppm->r = ppm_alloc_aligned(ppm->w, ppm->h);
				ppm->g = ppm_alloc_aligned(ppm->w, ppm->h);
				ppm->b = ppm_alloc_aligned(ppm->w, ppm->h);
				
				if (ppm->r == NULL || ppm->g == NULL || ppm->b == NULL)
				{
					#ifdef DEBUG
					printf("Error allocating memory\n");
					#endif
					return NULL;
				}
				
				fstart--;
			}
			else if (fstart == 1)
			{
				// Read bit depth
				ppm->max = atoi(hd);
				
				#ifdef DEBUG
				printf("- Bit depth: %d bpp\n", 3*8*(ppm->max / 255));
				#endif
				
				fstart--;
			}
		}
	}
	
	// Read image data
	int sz = ppm->w * ppm->h;
	int szpp = ceil(ppm->max / 255);
	
	void* imgdata = calloc(sz * 3, sizeof(uint8_t) * szpp);
	int singlebyte = ppm->max < 256;
	
	uint8_t* imgdata8 = (uint8_t*) imgdata;
	uint16_t* imgdata16 = (uint16_t*) imgdata;
	float corr = 1 / ppm->max;

	// Buffer file data
	fread(imgdata, (singlebyte) ? sizeof(uint8_t) : sizeof(uint16_t), sz * 3, fd);
	
	// Copy into ppm struct
	int i = 0;
	int j = 0;
	int n = 0;
	
	for (i = 0; i < ppm->h; i++)
	{
		for (j = 0; j < ppm->w; j++)
		{
			ppm->r[i + ppm->h*j] = singlebyte ? imgdata8[n++] : (imgdata16[n+=szpp] * corr * 255);
			ppm->g[i + ppm->h*j] = singlebyte ? imgdata8[n++] : (imgdata16[n+=szpp] * corr * 255);
			ppm->b[i + ppm->h*j] = singlebyte ? imgdata8[n++] : (imgdata16[n+=szpp] * corr * 255);
		}
	}
	
	ppm->max = 255;
	
	free(imgdata);
	fclose(fd);
	
	return ppm;
}


inline uint8_t* ppm_alloc_aligned(uint16_t w, uint16_t h)
{
	uint8_t* ptr = NULL;
	
	#ifdef C11
		ptr = (uint8_t*)aligned_alloc(16, w * h * sizeof(uint8_t));
	#elif ICC
		ptr = (uint8_t*)_mm_malloc(w * h * sizeof(uint8_t), 16);
	#elif PX
		#if !defined(posix_memalign)
			posix_memalign((void**)&ptr, 16, w * h * sizeof(uint8_t));
		#elif !defined(aligned_alloc)
			ptr = (uint8_t*)aligned_alloc(16, w * h * sizeof(uint8_t));
		#elif !defined(memalign)
			ptr = (uint8_t*)memalign(16, w * h * sizeof(uint8_t));
		#else
			#error "Aligned memory allocation is not supported by the environment"
		#endif
	#endif
	
	return ptr;
}


ppm_t* ppm_create(uint16_t w, uint16_t h, uint16_t max)
{
	ppm_t* ppm = (ppm_t*)malloc(sizeof(ppm_t));
	
	ppm->h = h;
	ppm->w = w;
	ppm->max = max;
	
	ppm->r = ppm_alloc_aligned(ppm->w, ppm->h);
	ppm->g = ppm_alloc_aligned(ppm->w, ppm->h);
	ppm->b = ppm_alloc_aligned(ppm->w, ppm->h);
	
	return ppm;
}


void ppm_write(const char *fname, const ppm_t *ppm)
{
	FILE* fd = fopen(fname, "w");
	
	if (fd == NULL)
	{
		return;
	}
	
	fprintf(fd, "P6\n");
	fprintf(fd, "# HCAP Trabajo SSE\n");
	fprintf(fd, "%d %d\n", ppm->w, ppm->h);
	fprintf(fd, "%d\n", ppm->max);
	
	size_t sz = ppm->w * ppm->h * 3;
	uint8_t* dataptr = calloc(sz, sizeof(uint8_t));
	
	int i = 0;
	int j = 0;
	int n = 0;
	
	for (i = 0; i < ppm->h; i++)
	{
		for (j = 0; j < ppm->w; j++)
		{
			dataptr[n++] = ppm->r[i + ppm->h*j];
			dataptr[n++] = ppm->g[i + ppm->h*j];
			dataptr[n++] = ppm->b[i + ppm->h*j];
		}
	}
	
	
	
	fwrite(dataptr, sizeof(uint8_t), sz * 3, fd);
	fclose(fd);
	free(dataptr);
	
	return;
}


void ppm_free(ppm_t *ppm)
{
	if (ppm != NULL)
	{
		if (ppm->r != NULL)
		{
			#ifdef ICC
			_mm_free(ppm->r);
			#else
			free(ppm->r);
			#endif
		}

		if (ppm->g != NULL)
		{
			#ifdef ICC
			_mm_free(ppm->g);
			#else
			free(ppm->g);
			#endif
		}

		if (ppm->b != NULL)
		{
			#ifdef ICC
			_mm_free(ppm->b);
			#else
			free(ppm->b);
			#endif
		}

		free(ppm);
	}
}