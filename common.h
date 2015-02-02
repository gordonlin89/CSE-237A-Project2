#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


/*
 * path definitions
 */

#define CPU_GET "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"
#define CPU_PUT "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed"
#define CPU_UTIL "/proc/stat"

#define GPU_FREQ "/sys/class/kgsl/kgsl-3d0/gpuclk"
#define GPU_PUT "/sys/class/kgsl/kgsl-3d0/gpuclk"
#define GPU_PUT2 "/sys/class/kgsl/kgsl-3d0/max_gpuclk"
#define GPU_UTIL "/sys/class/kgsl/kgsl-3d0/gpubusy"

/*
 * cpu frequencies
 */

unsigned int cpu_freqs[] = {
	192000, 384000, 432000, 486000,  540000,
	594000, 648000, 702000, 756000,  810000,
	864000, 918000, 972000, 1026000, 1080000,
	1134000, 1188000
};

#define CPU_NFREQS (sizeof(cpu_freqs) / sizeof(unsigned int))
#define CPU_HI (CPU_NFREQS - 1)
#define CPU_MIN cpu_freqs[0]
#define CPU_MAX cpu_freqs[CPU_NFREQS - 1]

/*
 * gpu frequencies
 */

unsigned int gpu_freqs[] = {
	177778, 200000, 228571, 266667
};

#define GPU_NFREQS (sizeof(gpu_freqs) / sizeof(unsigned int))
#define GPU_HI (GPU_NFREQS - 1)
#define GPU_MIN gpu_freqs[0]
#define GPU_MAX gpu_freqs[GPU_NFREQS - 1]


/**
 * Check a CPU frequency to make sure it's valid.
 *   @f: The frequency.
 *   &returns: True if valid, false otherwise.
 */

bool cpu_chk(unsigned int f)
{
	unsigned int i;

	for(i = 0; i < CPU_NFREQS; i++) {
		if(cpu_freqs[i] == f)
			return true;
	}

	return false;
}


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


void cpu_init()
{
	FILE *fp;

	fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "w");
	fprintf(fp, "userspace");
	fclose(fp);

	fp = fopen("/sys/devices/system/cpu/cpu1/online", "w");
	fprintf(fp, "0");
	fclose(fp);
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
 * Set the CPU frequency.
 *   @freq: The frequency.
 */

void cpu_set(unsigned int freq)
{
	FILE *file;

	file = fopen(CPU_PUT, "w");
	fprintf(file, "%u", freq);
	fclose(file);
}

/**
 * Retrieve the CPU utilization.
 *   &returns: The utilization between '0.0' and '1.0'.
 */

float cpu_util()
{
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


void gpu_init()
{
	FILE *fp;

	fp = fopen("/sys/class/kgsl/kgsl-3d0/pwrscale/policy", "w");
	fprintf(fp, "none");
	fclose(fp);
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
 * Set the GPU frequency.
 *   @freq: The frequency.
 */

void gpu_set(unsigned int freq)
{
	FILE *fp;

	fp = fopen(GPU_PUT, "w");
	fprintf(fp, "%u", freq);
	fclose(fp);

	fp = fopen(GPU_PUT2, "w");
	fprintf(fp, "%u", freq);
	fclose(fp);
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


/**
 * Retrieve the system time in milliseconds.
 *   &returns: Time in milliseconds.
 */

unsigned long mstime()
{
	struct timespec spec;

	clock_gettime(CLOCK_REALTIME, &spec);

	return spec.tv_sec * 1000l + spec.tv_nsec / 1000000l;
}


unsigned long _dbgtime_start;

/**
 * Initialize the start time of the process.
 */

__attribute__((constructor))
void _dbgtime_init()
{
	_dbgtime_start = mstime();
}

/**
 * Retrieve the current time in the format '####.##'. Be aware, the returned
 * buffer is statically allocated.
 *   &returns: The formatted time.
 */

const char *dbgtime()
{
	static char buf[64];
	unsigned long tm = mstime() - _dbgtime_start;

	snprintf(buf, sizeof(buf), "%.04lu.%.02lu", tm / 1000, (tm % 1000) / 10);

	return buf;
}


unsigned int opt_uint(const char *arg)
{
	long val;

	if(arg == NULL)
		fprintf(stderr, "Missing parameter."), exit(1);

	val = strtol(arg, (char **)&arg, 0);
	if((*arg != '\0') || (val < 0) || (val >= UINT_MAX))
		fprintf(stderr, "Invalid number."), exit(1);

	return val;
}

float opt_float(const char *arg)
{
	float val;

	if(arg == NULL)
		fprintf(stderr, "Missing parameter."), exit(1);

	val = strtof(arg, (char **)&arg);
	if((*arg != '\0') || (val < 0))
		fprintf(stderr, "Invalid number."), exit(1);

	return val;
}
