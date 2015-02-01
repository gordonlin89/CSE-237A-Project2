PRE	= arm-linux-gnueabi-
CC	= $(PRE)gcc
LD	= $(PRE)gcc
CFLAGS  = -static -s -Os

all: gov.out

gov.out: gov.c Makefile
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -rf gov.out


run: gov.out
	adb push gov.out /data/local
