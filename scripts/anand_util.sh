#!/bin/sh

adb push event/event_3 /sdcard/events
adb shell '/data/local/periodic.out -exec "./data/local/event_replay" > /data/local/anand_util.csv'
adb pull /data/local/anand_util.csv

