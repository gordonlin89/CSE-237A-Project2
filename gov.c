#include "common.h"

unsigned int tm = 250;

int main()
{
	bool max = false;
	bool debug = true;

	cpu_set(CPU_MIN);
	cpu_util();

	while(true) {
		usleep(1000*tm);

		if(max) {
			if(cpu_util() < 0.5) {
				cpu_set(CPU_MIN);
				max = false;

				if(debug)
					printf("cpu set to MIN\n");
			}
		}
		else {
			if(cpu_util() > 0.5) {
				cpu_set(CPU_MAX);
				max = true;

				if(debug)
					printf("cpu set to MAX\n");
			}
		}
	}

	return 0;
}
