#include "common.h"
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

	unsigned int i = 1;
	int pid = -1;

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
		else
			fprintf(stderr, "Invalid argument '%s'.\n", argv[i]), exit(1);
	}
	
	while (true) {
		if(pid > 0) {
			if(waitpid(pid, NULL, WNOHANG) > 0)
				break;
		}

		printf("%s,%f,%f\n", dbgtime(), cpu_util(), gpu_util());
		fflush(stdout);
		usleep(1000*100);
	}
	return 0;
}

