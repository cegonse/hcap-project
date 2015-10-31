#include "ppm.h"
#include "image.h"

int main (int argc, char* argv[])
{
	char fname[128];
	char outname[128];
	
	if (argc < 3)
	{
		printf("usage: sharpen [input.ppm] [output.ppm]\n");
		exit(1);
	}
	
	strcpy(fname, argv[1]);
	strcpy(outname, argv[2]);
	
	ppm_t *image = ppm_read(fname);
	
	if (image == NULL)
	{
		exit(1);
	}

	image = img_sharpen(image, 4, NULL);
	
	ppm_write(outname, image);
	ppm_free(image);
	
	return 0;
}