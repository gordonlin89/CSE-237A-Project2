#!/bin/sh

adb push event/event_1b /sdcard/events
adb shell '/data/local/periodic.out -exec "./data/local/event_replay" > /data/local/angrybirds_util.csv'
adb pull /data/local/angrybirds_util.csv

