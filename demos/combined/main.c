#include "../../src/ppm.h"
#include "../../src/image.h"

int main (int argc, char* argv[])
{
	char fname[128];
	char outname[128];
	
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
	image = img_fast_brighten(image, 2, NULL);
	
	// Apply sharpen filter with k = 4
	image = img_fast_sharpen(image, 4, NULL);
	
	// Turn to black and white
	image = img_fast_bw(image, NULL);
	
	// Save the result to the disk
	ppm_write(outname, image);
	ppm_free(image);
	
	return 0;
}