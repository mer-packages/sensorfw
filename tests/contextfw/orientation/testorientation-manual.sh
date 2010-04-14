#!/bin/sh
##
## Copyright (C) 2010 Nokia. All rights reserved.
##
## Test different accelerometer input values manually.
## As a result, screen should rotate.
##
## Contact: Matias Muhonen <ext-matias.muhonen@nokia.com>
## Contact: Tapio Rantala <ext-tapio.rantala@nokia.com>
##

INPUT_DEV_NUM=`ls /sys/class/input/event?/device/name | xargs grep -i accelerometer | cut -c23`
INPUT_DEV=/dev/input/event${INPUT_DEV_NUM}
FIFO=/tmp/accelerometer

echo "Faking input device $INPUT_DEV"

if [ ! -e $FIFO ] ; then
	echo "Created $FIFO for testing"
	mkfifo $FIFO
fi

export FAKEOPEN_PATHS="$INPUT_DEV,$FIFO,accelerometer"

# Shutdown sensord
/sbin/initctl stop sensord > /dev/null 2>&1
sleep 2
killall sensord > /dev/null 2>&1

# Start sensord manually to load libsensorfakeopen.so
env LD_PRELOAD=libsensorfakeopen.so /usr/sbin/sensord > /tmp/sensord.test.log 2>&1 &

# If we don't input something to the FIFO sensord blocks indefinitely
echo "" > $FIFO
sleep 1
echo "" > $FIFO                                   
sleep 1                                           
echo "" > $FIFO 

# Now run datafaker to input some fake sensor data

echo "NEW ORIENTATION: Top side up"
echo "36 0 1008" | /usr/bin/datafaker $FIFO

sleep 10 

echo "NEW ORIENTATION: Right side up"
echo "936 162 180" | /usr/bin/datafaker $FIFO

sleep 10 

echo "NEW ORIENTATION: Bottom up"
echo "72 -990 -162" | /usr/bin/datafaker $FIFO

sleep 10

echo "NEW ORIENTATION: Left side up"
echo "-954 -90 -36" | /usr/bin/datafaker $FIFO

sleep 10  

# Restart sensord with upstart
echo "Restoring sensord"
killall sensord
sleep 2
/sbin/initctl start sensord
