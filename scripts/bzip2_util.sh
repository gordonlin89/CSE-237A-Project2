#!/bin/sh

adb shell '/data/local/periodic.out -exec "./data/local/bzip2/bzip2_base.arm-gcc42-05 /data/local/bzip2/dryer.jpg 2 > /data/local/bzip2/dryer.jpg.out 2>> /data/local/bzip2/dryer.jpg.err" > /data/local/bzip2_util.csv'
adb pull /data/local/bzip2_util.csv

