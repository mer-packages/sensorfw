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

TESTCONFIGSOURCE_RM680=/usr/share/sensorfw-tests/00-automatic-testing-rm680.conf
TESTCONFIGSOURCE_RM696=/usr/share/sensorfw-tests/00-automatic-testing-rm696.conf
TESTCONFIGTARGET=/etc/sensorfw/sensord.conf.d/00-automatic-testing.conf
DIVERTPATH=/tmp/fakedsensors
FIFODIVERTS="accelerometer als"
POLLDIVERTS="accelerometer"

PRODUCT=`sysinfo-tool -g /component/product`
TESTCONFIGSOURCE=$TESTCONFIGSOURCE_RM680
if [ "$PRODUCT" = "RM-696" ]; then
    TESTCONFIGSOURCE=$TESTCONFIGSOURCE_RM696
fi

### main code

case "$1" in
  "start")
  if [ -f $TESTCONFIGTARGET ]; then
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

  # Setup config for testing
  cp $TESTCONFIGSOURCE $TESTCONFIGTARGET

  initctl stop sensord
  sleep 2
  killall sensord
  #initctl start sensord
  sensord -c=$TESTCONFIGTARGET &

  sleep 0.5
  echo "" >$DIVERTPATH/accelerometer & { sleep 0.5; eval 'kill $!' &> /dev/null; }

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
  if [ ! -f $TESTCONFIGTARGET ]; then
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
  sleep 2
  killall sensord

  # Remove extra configuration file
  rm $TESTCONFIGTARGET

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
