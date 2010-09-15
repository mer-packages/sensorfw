#!/bin/sh
##
## Copyright (C) 2009-2010 Nokia Corporation
##
## Contact: Tapio Rantala <ext-tapio.rantala@nokia.com>
##
## This file is part of Sensord.
##
## Sensord is free software; you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License
## version 2.1 as published by the Free Software Foundation.
##
## Sensord is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
##
##


### configuration

DIVERTPATH=/tmp/fakedsensors
FIFODIVERTS="accelerometer als"
POLLDIVERTS="accelerometer"

### main code

case "$1" in
  "start")
  if [ -f $DIVERTPATH/sensord.conf ]; then
    echo "Sensor diversion seems to be active already, quitting..."
    exit 1
  fi
  echo "Creating sensor diversions..."
  mkdir -p $DIVERTPATH || { echo Failed to create path $DIVERTPATH ; exit 1; }
  for i in $FIFODIVERTS; do
    mkfifo $DIVERTPATH/$i || { echo Failed to create fifo $DIVERTPATH/$i ; exit 1; }
  done

  for i in $POLLDIVERTS; do
    echo 0 > ${DIVERTPATH}/${i}_poll_rate || { echo Failed to create file ${DIVERTPATH}/${i}_poll_rate ; exit 1; }
  done

  mv /etc/sensord.conf $DIVERTPATH/ || { echo Failed to move sensord.conf ; exit 1; }
  echo "device_sys_path = /dev/input/event%1
device_poll_file_path = /sys/class/input/input%1/poll

# Proximity adaptor
proximity_dev_path = /dev/bh1770glc_ps

# ALS adaptor
als_dev_path = $DIVERTPATH/als

# Magnetometer drivers
mag_ak8974_dev_path = /dev/ak89740
mag_ak8975_dev_path = /dev/ak89750

dev_accelerometer = $DIVERTPATH/accelerometer
dev_poll_accelerometer = $DIVERTPATH/accelerometer_poll_rate

acc_trans_matrix = \"-1,0,0,0,-1,0,0,0,-1\"

magnetometer_scale_coefficient = 300

deviceId = default

[default]
accelerometeradaptor = accelerometeradaptor

[n900]
accelerometeradaptor = accelerometeradaptor-n900" > /etc/sensord.conf

  initctl stop sensord
  killall sensord
  initctl start sensord
  #/usr/sbin/sensord < /dev/null &
  sleep 0.5
  for i in 1 2; do
    echo "" >$DIVERTPATH/accelerometer & { sleep 0.5; eval 'kill $!' &> /dev/null; }
  done

  sleep 0.5
  result=`qdbus --system | grep -i sensor`
  if [ x"$result" == x ]; then
    echo "Sensord did not start: failed to register the D-Bus interface"
    exit 1
  else
    echo "$result"
    echo "Sensord started..."
  fi
  ;;

  "stop")
  if [ ! -f $DIVERTPATH/sensord.conf ]; then
    echo "Sensor diversion seems not to be active, quitting..."
    exit 1
  fi
  echo "Removing sensor diversions..."
  status=0
  if [[ 1 -ne `ps | grep -v grep | grep -c /usr/sbin/sensord` ]]; then
    echo "Sensord should be up, but it's not!"
    status=1
  fi
  initctl stop sensord
  killall sensord
  mv $DIVERTPATH/sensord.conf /etc/
  for i in $FIFODIVERTS; do
    rm $DIVERTPATH/$i
  done

  for i in $POLLDIVERTS; do
    rm ${DIVERTPATH}/${i}_poll_rate
  done
  initctl start sensord
  exit $status
  ;;

  *)
  echo "ERROR: Unrecognized parameters"
  echo "Use \"$0 start\" to divert sensor input from real sensors"
  echo "Use \"$0 stop\" to restore normal functionality"
  exit 1
  ;;
esac

exit 0
