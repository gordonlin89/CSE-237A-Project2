#!/bin/sh

adb push event/event_2 /sdcard/events
adb shell '/data/local/periodic.out -exec "./data/local/event_replay" > /data/local/purch_util.csv'
adb pull /data/local/purch_util.csv

