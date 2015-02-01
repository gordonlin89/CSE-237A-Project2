#include "common.h"


int main()
{
	cpu_util();

	while(true) {
		printf("freq: %u\n", cpu_get());
		printf("util: %f\n", cpu_util());
		printf("gpu-freq: %u\n", gpu_get());
		printf("gpu-util: %f\n", gpu_util());
		usleep(900000);
	}

	return 0;
}
