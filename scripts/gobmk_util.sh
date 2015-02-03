#!/bin/sh

adb shell '/data/local/periodic.out -exec "./data/local/gobmk/gobmk_base.arm-gcc42-05 --quiet --mode gtp < ./data/local/gobmk/dniwog.tst > ./data/local/gobmk/dniwog.out 2>> ./data/local/gobmk/dniwog.err" > /data/local/gobmk_util.csv'
adb pull /data/local/gobmk_util.csv

