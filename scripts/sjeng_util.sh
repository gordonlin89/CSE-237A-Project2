#!/bin/sh

adb shell '/data/local/periodic.out -exec "./data/local/sjeng/sjeng_base.arm-gcc42-05 ./data/local/sjeng/test.txt > ./data/local/sjeng/test.out 2>> ./data/local/sjeng/test.err" > /data/local/sjeng_util.csv'
adb pull /data/local/sjeng_util.csv
