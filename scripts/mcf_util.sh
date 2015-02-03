#!/bin/sh

adb shell '/data/local/periodic.out -exec "./data/local/mcf/mcf_base.arm-gcc42-05 ./data/local/mcf/inp_test.in > ./data/local/mcf/inp.out 2>> ./data/local/mcf/inp.err" > /data/local/mcf_util.csv'
adb pull /data/local/mcf_util.csv

