#include "common.h"
#include <poll.h>

unsigned int period[] = {
	250, 235, 220, 205, 190,
	180, 170, 160, 150, 145,
	140, 135, 130, 125, 120,
	110, 100
};

int main(int argc, char **argv)
{
	float c_util = 0.0f, g_util = 0.0f;
	int c_sel = 0, g_sel = 0;
	int debug = 0;
	unsigned int i = 1;
	float smooth = 0.8f;
	unsigned int inc = 3, dec = 1;
	float thres_low = 0.3, thres_high = 0.9;
	float gpu_low = 0.2, gpu_high = 0.6;
	bool c_enable = true, g_enable = true;

	while(argv[i] != NULL) {
		if(strcmp(argv[i], "-inc") == 0) {
			inc = opt_uint(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-dec") == 0) {
			dec = opt_uint(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-low") == 0) {
			thres_low = opt_float(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-gpuhigh") == 0) {
			gpu_high = opt_float(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-gpulow") == 0) {
			gpu_low = opt_float(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-high") == 0) {
			thres_high = opt_float(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-smooth") == 0) {
			smooth = opt_float(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-nocpu") == 0)
			c_enable = false, i++;
		else if(strcmp(argv[i], "-nogpu") == 0)
			g_enable = false, i++;
		else if(strcmp(argv[i], "-debug") == 0)
			debug++, i++;
		else
			fprintf(stderr, "Invalid argument '%s'.\n", argv[i]), exit(1);
	}

	cpu_init();
	cpu_set(CPU_MIN);
	cpu_util();

	gpu_init();
	gpu_set(GPU_MIN);
	gpu_util();

	while(true) {
		struct pollfd fds;

		fds.fd = STDIN_FILENO;
		fds.events = POLLIN;
		fds.revents = 0;

		if(poll(&fds, 1, period[c_sel]) > 0)
			break;

		c_util = (1.0 - smooth) * cpu_util() + smooth * c_util;
		g_util = (1.0 - smooth) * gpu_util() + smooth * g_util;

		if(debug >= 2)
			printf("%s: cpu %.03f, gpu %.03f\n", dbgtime(), c_util, g_util);

		if(c_enable && (c_util > thres_high) && (c_sel < CPU_HI)) {
			c_sel += inc;
			if(c_sel > CPU_HI)
				c_sel = CPU_HI;

			cpu_set(cpu_freqs[c_sel]);
			if(debug >= 1)
				printf("%s: cpu set to %d\n", dbgtime(), cpu_freqs[c_sel]);
		}
		else if(c_enable && (c_util < thres_low) && (c_sel > 0)) {
			c_sel -= dec;
			if(c_sel < 0)
				c_sel = 0;

			cpu_set(cpu_freqs[c_sel]);
			if(debug >= 1)
				printf("%s: cpu set to %d\n", dbgtime(), cpu_freqs[c_sel]);
		}

		if(g_enable && (g_util > gpu_high) && (g_sel < GPU_HI)) {
			g_sel += 1;
			if(g_sel > GPU_HI)
				g_sel = GPU_HI;

			gpu_set(gpu_freqs[g_sel]);
			if(debug >= 1)
				printf("%s: gpu set to %d\n", dbgtime(), gpu_freqs[g_sel]);
		}
		else if(g_enable && (g_util < gpu_low) && (g_sel > 0)) {
			g_sel -= 1;
			if(g_sel < 0)
				g_sel = 0;

			gpu_set(gpu_freqs[g_sel]);
			if(debug >= 1)
				printf("%s: gpu set to %d\n", dbgtime(), gpu_freqs[g_sel]);
		}
	}

	return 0;
}
