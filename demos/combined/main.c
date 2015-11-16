#include "../../src/ppm.h"
#include "../../src/image.h"

int main (int argc, char* argv[])
{
	char fname[128];
	char outname[128];
	uint64_t cycles = 0;
	
	if (argc < 3)
	{
		printf("usage: combined [input.ppm] [output.ppm]\n");
		exit(1);
	}
	
	strcpy(fname, argv[1]);
	strcpy(outname, argv[2]);
	
	// Attempt to load the input image
	ppm_t *image = ppm_read(fname);
	
	if (image == NULL)
	{
		exit(1);
	}
	
	// Brighten image
	img_brighten(image, 2, &cycles);
	printf("Scalar brighten took %d cycles.\n", (int)cycles);
	
	img_fast_brighten(image, 2, &cycles);
	printf("SSE brighten took %d cycles.\n", (int)cycles);
	
	// Apply sharpen filter with k = 4
	img_sharpen(image, 4, &cycles);
	printf("Scalar convolution took %d cycles.\n", (int)cycles);
	
	img_fast_sharpen_copy(image, 4, &cycles);
	printf("SSE convolution took %d cycles.\n", (int)cycles);
	
	// Turn to black and white
	img_bw(image, &cycles);
	printf("Scalar black and white took %d cycles.\n", (int)cycles);
	
	img_fast_bw(image, &cycles);
	printf("SSE black and white took %d cycles.\n", (int)cycles);
	
	// Save the result to the disk
	ppm_write(outname, image);
	ppm_free(image);
	
	return 0;
}