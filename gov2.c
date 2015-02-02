#include "common.h"

float thres_low = 0.3, thres_high = 0.9;
unsigned int inc = 3;
unsigned int dec = 1;
unsigned int tm[] = {
	250, 235, 220, 205, 190,
	180, 170, 160, 150, 145,
	140, 135, 130, 125, 120,
	110, 100
};
float smooth = 0.8f;

int main()
{
	float util = 0.0;;
	int sel = 0;
	bool debug = true;

	cpu_init();
	cpu_set(CPU_MIN);
	cpu_util();

	while(true) {
		usleep(1000*tm[sel]);

		util = (1.0 - smooth) * cpu_util() + smooth * util;
		if(util > thres_high) {
			sel += inc;
			if(sel > CPU_HI)
				sel = CPU_HI;

			cpu_set(cpu_freqs[sel]);
			if(debug)
				printf("inc; set to %f MHz\n", cpu_freqs[sel] / 1.000f);
		}
		else if(util < thres_low) {
			sel -= dec;
			if(sel < 0)
				sel = 0;

			cpu_set(cpu_freqs[sel]);
			if(debug)
				printf("dec; set to %f MHz\n", cpu_freqs[sel] / 1.000f);
		}
	}

	return 0;
}
