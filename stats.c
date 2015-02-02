#include "common.h"
#include <poll.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>

extern char **environ;


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

unsigned int gpu_idle(unsigned int freq)
{
	switch(freq) {
	case 177778: return 201;
	case 200000: return 250;
	case 228571: return 280;
	case 266667: return 333;
	}

	error("Invalid GPU frequency.");
}

unsigned int gpu_active(unsigned int freq)
{
	switch(freq) {
	case 177778: return 614;
	case 200000: return 620;
	case 228571: return 653;
	case 266667: return 747;
	}

	error("Invalid GPU frequency.");
}


int main(int argc, char **argv)
{
	struct pollfd fds;
	float energy = 0.0;
	unsigned int i = 1, n = 0, period = 500;
	int pid = -1;
	float step;
	bool debug = false;

	while(argv[i] != NULL) {
		if(strcmp(argv[i], "-exec") == 0) {
			if(argv[i+1] == NULL)
				fprintf(stderr, "Missing program name.\n"), exit(1);

			pid = fork();
			if(pid == 0) {
				if(execl("/system/bin/sh", "sh", "-c", argv[i+1], NULL) < 0)
					fprintf(stderr, "Failed to execute program. %s.\n", strerror(errno)), exit(1);
			}

			i+= 2;
		}
		else if(strcmp(argv[i], "-period") == 0) {
			period = opt_uint(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-debug") == 0)
			debug = true, i++;
		else
			fprintf(stderr, "Invalid argument '%s'.\n", argv[i]), exit(1);
	}

	step = (float)period / 1000.0f;
	cpu_util();

	while(true) {
		float cur;
		unsigned int freq, g_freq;

		if(pid > 0) {
			if(waitpid(pid, NULL, WNOHANG) > 0)
				break;
		}

		fds.fd = STDIN_FILENO;
		fds.events = POLLIN;
		fds.revents = 0;

		if(poll(&fds, 1, period) > 0)
			break;

		n++;

		freq = cpu_get();
		g_freq = cpu_get();

		cur = pwr_idle(freq) * step + pwr_active(freq) * cpu_util() * step;
		cur += gpu_idle(g_freq) * step + gpu_active(g_freq) * gpu_util() * step;

		energy += cur;

		if(debug)
			printf("%s: %u %u %f\n", dbgtime(), freq, g_freq, cur);
	}

	printf("total: %f\n", energy);
	printf("ave: %f\n", energy / n / step);
	printf("time: %f\n", n * step);
	printf("delay-product: %f\n", energy * n * step);

	return 0;
}
