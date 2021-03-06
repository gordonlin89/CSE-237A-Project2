PRE	= arm-linux-gnueabi-
CC	= $(PRE)gcc
LD	= $(PRE)gcc
CFLAGS  = -static -s -Os -Wall

all: gov.out gov2.out mock2.out stats.out periodic.out

gov.out: gov.c Makefile common.h
	$(CC) $(CFLAGS) $< -o $@

mock2.out: mock2.c Makefile common.h
	gcc $(CFLAGS) $< -o $@

gov2.out: gov2.c Makefile common.h
	$(CC) $(CFLAGS) $< -o $@

stats.out: stats.c Makefile common.h
	$(CC) $(CFLAGS) $< -o $@

periodic.out: periodic.c Makefile common.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.out

run: all
	adb push gov.out /data/local
	adb push gov2.out /data/local
	adb push stats.out /data/local
	adb push periodic.out /data/local

