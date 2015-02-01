PRE	= arm-linux-gnueabi-
CC	= $(PRE)gcc
LD	= $(PRE)gcc
CFLAGS  = -static -s -Os

all: gov.out gov2.out stats.out

gov.out: gov.c Makefile
	$(CC) $(CFLAGS) $< -o $@

gov2.out: gov2.c Makefile
	$(CC) $(CFLAGS) $< -o $@

stats.out: stats.c Makefile
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.out


run: all
	adb push gov.out /data/local
	adb push gov2.out /data/local
	adb push stats.out /data/local
