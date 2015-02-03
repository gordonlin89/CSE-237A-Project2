#!/bin/sh

adb push event/event_5 /sdcard/events
adb shell '/data/local/periodic.out -exec "./data/local/event_replay" > /data/local/neocore_util.csv'
adb pull /data/local/neocore_util.csv

