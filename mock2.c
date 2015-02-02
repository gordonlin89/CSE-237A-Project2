#include "common.h"
#include <poll.h>

static int cnt = 0;

unsigned int period[] = {
	250, 235, 220, 205, 190,
	180, 170, 160, 150, 145,
	140, 135, 130, 125, 120,
	110, 100
};

unsigned int mock_val;

void mock_init()
{
	mock_val = CPU_MIN;
}

float mock_util(int mock_sel)
{
	float val;
	float fullscale = (float)CPU_MAX / (float)mock_val;

	switch(mock_sel) {
	case 1:
		val = (cnt >= 30 && cnt < 120) ? 1.0f : 0.0f;
		break;

	case 2:
		if((cnt >= 55 && cnt <= 56) || (cnt >= 92 && cnt <= 96) || (cnt >= 121 && cnt <= 121))
			val = 0.04;
		else
			val = (cnt >= 30 && cnt < 130) ? 1.0f : 0.0f;
		break;

	case 3:
		if(cnt >= 20 && cnt < 120)
			val = 1.0f / 100.0f * (cnt - 20) * fullscale;
		else
			val = 0.0f;

		break;

	default:
		val = 0.0f;
	}

	if(val > 1.0f)
		val = 1.0f;

	return val;
}

void mock_set(unsigned int freq)
{
	mock_val = freq;
}

unsigned int mock_get()
{
	return mock_val;
}

int main(int argc, char **argv)
{
	float c_util = 0.0f;
	int c_sel = 0;
	int debug = 0;
	unsigned int i = 1;
	float smooth = 0.6f;
	unsigned int inc = 4, dec = 1;
	float thres_low = 0.3, thres_high = 0.7;
	bool c_enable = true;
	int mock_sel = 1;

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
		else if(strcmp(argv[i], "-high") == 0) {
			thres_high = opt_float(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-smooth") == 0) {
			smooth = opt_float(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-mock") == 0) {
			mock_sel = opt_uint(argv[i+1]);
			i += 2;
		}
		else if(strcmp(argv[i], "-nocpu") == 0)
			c_enable = false, i++;
		else if(strcmp(argv[i], "-debug") == 0)
			debug++, i++;
		else
			fprintf(stderr, "Invalid argument '%s'.\n", argv[i]), exit(1);
	}

	mock_init();
	mock_set(CPU_MIN);
	mock_util(mock_sel);

	while(true) {
		struct pollfd fds;

		fds.fd = STDIN_FILENO;
		fds.events = POLLIN;
		fds.revents = 0;

		if(poll(&fds, 1, period[c_sel]/5) > 0)
			break;

		c_util = (1.0 - smooth) * mock_util(mock_sel) + smooth * c_util;

		if(debug >= 2)
			printf("%s: cpu %.03f\n", dbgtime(), c_util);

		if(c_enable && (c_util > thres_high) && (c_sel < CPU_HI)) {
			c_sel += inc;
			if(c_sel > CPU_HI)
				c_sel = CPU_HI;

			mock_set(cpu_freqs[c_sel]);
			if(debug >= 1)
				printf("%s: cpu set to %d\n", dbgtime(), cpu_freqs[c_sel]);
		}
		else if(c_enable && (c_util < thres_low) && (c_sel > 0)) {
			c_sel -= dec;
			if(c_sel < 0)
				c_sel = 0;

			mock_set(cpu_freqs[c_sel]);
			if(debug >= 1)
				printf("%s: cpu set to %d\n", dbgtime(), cpu_freqs[c_sel]);
		}

		printf("%s\t%f\t%f\t%u\n", dbgtime(), mock_util(mock_sel), c_util, mock_get());
		if(cnt++ > 160)
			break;
	}

	return 0;
}
