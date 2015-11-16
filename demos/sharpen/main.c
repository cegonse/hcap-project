#include "../../src/ppm.h"
#include "../../src/image.h"

int main (int argc, char* argv[])
{
	char fname[128];
	char outname[128];
	uint64_t cycles = 0, cyclesSse = 0, startTSC, stopTSC, cycleCnt, clksPerMicro;
	float clkRate;
	ppm_t *image, *temp;
	
	if (argc < 3)
	{
		printf("usage: sharpen [input.ppm] [output.ppm]\n");
		exit(1);
	}
	
	strcpy(fname, argv[1]);
	strcpy(outname, argv[2]);
	
	// Attempt to load the input image
	image = ppm_read(fname);
	
	if (image == NULL)
	{
		exit(1);
	}
	
	temp = ppm_create(image->w, image->h, image->max);
	
	if (temp == NULL)
	{
		ppm_free(image);
		exit(1);
	}
	
	// Estimate CPU clock rate
    startTSC = readTSC();
    usleep(1000000);
    stopTSC = readTSC();
    cycleCnt = cyclesElapsed(stopTSC, startTSC);

    clkRate = ((float)cycleCnt)/1000000.0;
    clksPerMicro = (uint64_t)clkRate;
    printf("Based on usleep accuracy, CPU clk rate = %llu clks/sec,",cycleCnt);
    printf(" %7.1f Mhz\n", clkRate);
	
	// Apply sharpen filter with k = 4
	img_sharpen(image, 4, &cycles);
	printf("Scalar convolution loop took %llu cycles.\n", cycles);
	
	img_fast_sharpen_copy(image, 4, &cyclesSse);
	printf("SSE convolution loop (with copy) took %llu cycles.\n", cyclesSse);
	
	img_fast_sharpen(image, temp, 4, &cyclesSse);
	printf("SSE convolution loop (without copy) took %llu cycles.\n", cyclesSse);
	
	printf("Speed-up: %.2f\n", (double)cyclesSse / (double)cycles);
	
	// Save the result to the disk
	ppm_write(outname, temp);
	ppm_free(image);
	ppm_free(temp);
	
	return 0;
}