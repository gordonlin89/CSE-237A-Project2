#include "common.h"
#include <poll.h>


int tm = 500;


unsigned int pwr_idle(unsigned int freq)
{
	switch(freq) {
	case 1188000: return 23;
	case 1134000: return 21;
	case 1080000: return 19;
	case 1026000: return 18;
	case 972000: return 16;
	case 918000: return 16;
	case 864000: return 15;
	case 810000: return 15;
	case 756000: return 14;
	case 702000: return 14;
	case 648000: return 13;
	case 594000: return 12;
	case 540000: return 12;
	case 486000: return 10;
	case 432000: return 10;
	case 384000: return 10;
	case 192000: return 9;
	}

	error("Invalid frequency.");
}

unsigned int pwr_active(unsigned int freq)
{
	switch(freq) {
	case 1188000: return 262;
	case 1134000: return 252;
	case 1080000: return 231;
	case 1026000: return 210;
	case 972000: return 199;
	case 918000: return 184;
	case 864000: return 168;
	case 810000: return 157;
	case 756000: return 136;
	case 702000: return 126;
	case 648000: return 115;
	case 594000: return 105;
	case 540000: return 94;
	case 486000: return 84;
	case 432000: return 73;
	case 384000: return 63;
	case 192000: return 37;
	}

	error("Invalid frequency.");
}


int main()
{
	struct pollfd fds;
	float pwr = 0.0;
	float step = (float)tm / 1000.0f;
	unsigned int n = 0;

	cpu_util();

	while(true) {
		unsigned int freq, idle, active;

		fds.fd = STDIN_FILENO;
		fds.events = POLLIN;
		fds.revents = 0;

		if(poll(&fds, 1, tm) > 0)
			break;

		n++;
		freq = cpu_get();
		pwr += pwr_idle(freq) * step + pwr_active(freq) * cpu_util() * step;

		//printf("freq: %u\n", cpu_get());
		//printf("util: %f\n", cpu_util());
		//printf("gpu-freq: %u\n", gpu_get());
		//printf("gpu-util: %f\n", gpu_util());
	}

	printf("total: %f\n", pwr);
	printf("ave: %f\n", pwr / n / step);
	printf("time: %f\n", n * step);

	return 0;
}
