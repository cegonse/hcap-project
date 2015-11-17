#include "../../src/ppm.h"
#include "../../src/image.h"

int main (int argc, char* argv[])
{
	char fname[128], seqname[128], cpname[128], wocpname[128];
	uint64_t cycles = 0, cyclesCopy = 0, cyclesWo = 0, startTSC, stopTSC, cycleCnt, clksPerMicro;
	float clkRate;
	ppm_t *original, *sequential, *wcopy, *wocopy;
	
	if (argc < 5)
	{
		printf("usage: sharpen [input.ppm] [sequential.ppm] [with_copy.ppm] [without_copy.ppm]\n");
		exit(1);
	}
	
	strcpy(fname, argv[1]);
	strcpy(seqname, argv[2]);
	strcpy(cpname, argv[3]);
	strcpy(wocpname, argv[4]);
	
	// Attempt to load the input image
	original = ppm_read(fname);
	
	if (original == NULL)
	{
		exit(1);
	}
	
	// Create the three modified images
	sequential = ppm_create(original->w, original->h, original->max);
	wcopy = ppm_create(original->w, original->h, original->max);
	wocopy = ppm_create(original->w, original->h, original->max);
	
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
	img_sharpen(original, sequential, 4, &cycles);
	printf("Scalar convolution loop took %llu cycles.\n", cycles);
	
	img_fast_sharpen_copy(original, wcopy, 4, &cyclesCopy);
	printf("SSE convolution loop (with copy) took %llu cycles.\n", cyclesCopy);
	
	img_fast_sharpen(original, wocopy, 4, &cyclesWo);
	printf("SSE convolution loop (without copy) took %llu cycles.\n", cyclesWo);
	
	printf("w/Copy Speed-up: %.2f%% \n", 100.0*(1.0 - (double)cyclesCopy / (double)cycles));
	printf("wo/Copy Speed-up: %.2f%% \n", 100.0*(1.0 - (double)cyclesWo / (double)cycles));
	
	// Save the result to the disk
	ppm_write(seqname, sequential);
	ppm_write(cpname, wcopy);
	ppm_write(wocpname, wocopy);
	
	ppm_free(original);
	ppm_free(sequential);
	ppm_free(wcopy);
	ppm_free(wocopy);
	
	return 0;
}