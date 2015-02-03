#!/bin/sh

adb push event/event_4 /sdcard/events
adb shell '/data/local/periodic.out -exec "./data/local/event_replay" > /data/local/imgur_util.csv'
adb pull /data/local/imgur_util.csv
