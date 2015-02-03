#include "common.h"
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <poll.h>

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
	case 177778000: return 201;
	case 200000000: return 250;
	case 228571000: return 280;
	case 266667000: return 333;
	}

	fprintf(stderr, "Invalid GPU frequency (%u).", freq);
	abort();
}

unsigned int gpu_active(unsigned int freq)
{
	switch(freq) {
	case 177778000: return 614;
	case 200000000: return 620;
	case 228571000: return 653;
	case 266667000: return 747;
	}

	fprintf(stderr, "Invalid GPU frequency (%u).", freq);
	abort();
}

int main(int argc, char* argv[]) {

	/*
		Parse args.
	*/
	unsigned int i = 1;
	int pid = -1;

	while (argv[i] != NULL) {
		if (strcmp(argv[i], "-exec") == 0) {
			if (argv[i+1] == NULL) {
				fprintf(stderr, "Missing program name.\n"), exit(1);
			}
			pid = fork();
			if (pid == 0) {
				if(execl("/system/bin/sh", "sh", "-c", argv[i+1], NULL) < 0) {
					fprintf(stderr, "Failed to execute program. %s.\n", strerror(errno)), exit(1);
				}
			}
			i+= 2;
		}
		else {
			fprintf(stderr, "Invalid argument '%s'.\n", argv[i]), exit(1);
		}
	}

	/*
		Collection of data at every time step.
	*/
	struct pollfd fds;
	unsigned int cpu_freq, gpu_freq;
	float cpu_utilization, gpu_utilization;
	float cpu_power, gpu_power;
	
	fds.fd = STDIN_FILENO;
	fds.events = POLLIN;
	fds.revents = 0;

	printf("%s,%s,%s,%s,%s,%s,%s\n", "timestep", "cpu_freq", "cpu_util", "cpu_power", "gpu_freq", "gpu_util", "gpu_power");
	
	while (true) {
		//Compute metrics
		cpu_freq = cpu_get();
		cpu_utilization = cpu_util();
		cpu_power = pwr_idle(cpu_freq) + pwr_active(cpu_freq) * cpu_utilization;

		gpu_freq = gpu_get();
		gpu_utilization = gpu_util();
		gpu_power = gpu_idle(gpu_freq) + gpu_active(gpu_freq) * gpu_utilization;

		//Print data
		printf("%s,%u,%f,%f,%u,%f,%f\n", dbgtime(), cpu_freq, cpu_utilization, cpu_power, gpu_freq, gpu_utilization, gpu_power);
		fflush(stdout);

		if (pid > 0 && waitpid(pid, NULL, WNOHANG) > 0) {
			break;
		}
		if (poll(&fds, 1, 100) > 0) {
			break;
		}
	}
	return 0;
}

