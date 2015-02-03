#!/bin/sh

adb shell '/data/local/periodic.out -exec "./data/local/hmmer/hmmer_base.arm-gcc42-05 --fixed 0 --mean 325 --num 45000 --sd 200 --seed 0 ./data/local/hmmer/bombesin.hmm > ./data/local/hmmer/bombesin.out 2>> ./data/local/hmmer/bombesin.err" > /data/local/hmmer_util.csv'
adb pull /data/local/hmmer_util.csv

