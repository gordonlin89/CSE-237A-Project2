#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>


/*
 * path definitions
 */

#define CPU_GET "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"
#define CPU_UTIL "/proc/stat"

#define GPU_FREQ "/sys/class/kgsl/kgsl-3d0/gpuclk"
#define GPU_UTIL "/sys/class/kgsl/kgsl-3d0/gpubusy"


/**
 * Print an error message to the stderr and terminate.
 *   @msg: The message.
 */

__attribute__((noreturn)) void error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	abort();
}

/**
 * Read a file, given the path, at most 'size' bytes into the file. THe buffer
 * will always be null delimited.
 *   @path: The path.
 *   @buf: The buffer.
 *   @size: The number of bytes to read.
 */

void readfile(const char *path, char *buf, size_t size)
{
	int fd;
	ssize_t rd;

	fd = open(path, O_RDONLY);
	if(fd < 0)
		error("Failed to open file.");

	rd = read(fd, buf, size - 1);
	if(rd < 0)
		error("Failed to read file.");

	close(fd);

	buf[rd] = '\0';
}


/**
 * Retrieve the CPU frequency.
 *   &returns: The frequncy.
 */

unsigned int cpu_get()
{
	char buf[256];
	unsigned int freq;

	readfile(CPU_GET, buf, sizeof(buf));
	sscanf(buf, "%u", &freq);

	return freq;
}

/**
 * Retrieve the CPU utilization.
 *   &returns: The utilization between '0.0' and '1.0'.
 */

float cpu_util()
{
	int fd;
	ssize_t rd;
	float util;
	char *ptr, buf[512], str[32];
	unsigned int n, user, nice, sys, idle, iowait, irq, sirq;
	static unsigned int s_user, s_nice, s_sys, s_idle, s_iowait, s_irq, s_sirq;

	readfile(CPU_UTIL, buf, sizeof(buf));

	ptr = buf;
	while((sscanf(ptr, "%31s%n", str, &n) > 0) && (strcmp(str, "cpu0") != 0))
		ptr += n;

	sscanf(ptr + n, "%u %u %u %u %u %u %u", &user, &nice, &sys, &idle, &iowait, &irq, &sirq);

	n = user + nice + sys - s_user - s_nice - s_sys;
	//printf("%u / %u\n", n, n + idle + iowait + irq + sirq - s_idle - s_iowait - s_irq - s_sirq);
	util = n ? (float)n / (float)(n + idle + iowait + irq + sirq - s_idle - s_iowait - s_irq - s_sirq) : 0.0f;

	s_user = user;
	s_nice = nice;
	s_sys = sys;
	s_idle = idle;
	s_iowait = iowait;
	s_irq = irq;
	s_sirq = sirq;

	return util;
}

/**
 * Retrieve the GPU frequency.
 *   &returns: The frequncy.
 */

unsigned int gpu_get()
{
	char buf[256];
	unsigned int freq;

	readfile(GPU_FREQ, buf, sizeof(buf));
	sscanf(buf, "%u", &freq);

	return freq;
}

/**
 * Retrieve the GPU utilization.
 *   &returns: The utilization between '0.0' and '1.0'.
 */

float gpu_util()
{
	char buf[512];
	unsigned int a, b;

	readfile(GPU_UTIL, buf, sizeof(buf));
	sscanf(buf, "%u%u", &a, &b);

	return b ? (float)a / (float)b : 0.0f;
}
