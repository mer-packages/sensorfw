#!/usr/bin/env python
##
## Copyright (C) 2009-2010 Nokia Corporation
##
## Contact: Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

import re, string, sys, os, time, signal, unittest

appName = 'sensord-deadclient'
timeToSleep = 10
signalForKilling = signal.SIGSEGV
logFile = '/var/log/syslog'

def timeoutHandler(signum, frame):
    raise Exception('Tests have been running for too long')

class DeadClient(unittest.TestCase):
    def setUp(self):
        self.file = open(logFile)

        #read the file to the bottom first
        while 1:
            lines = self.file.readlines(100000)
            if not lines:
                break
            for line in lines:
                pass # do something

    def test_TwoClientsAndOneCrash(self):
        success = True
        print("Launching " + appName + "...")
        childPid = os.spawnlpe(os.P_NOWAIT, appName, appName, '-c', os.environ)
        print('Child process is: ' + repr(childPid))
        time.sleep(timeToSleep)
        print('Starting the second client..')
        childPid2 = os.spawnlpe(os.P_NOWAIT, appName, appName, os.environ)
        print('Second child process is: ' + repr(childPid2))
        print('Sleeping ' + repr(timeToSleep) + ' seconds before killing...')
        time.sleep(timeToSleep)

        print('Killing the first process with signal ' + repr(signalForKilling) + ' now...')
        os.kill(childPid, signalForKilling)

        #start reading the log file again, and filter the lines about sensord
        while 1:
            lines = self.file.readlines(100000)
            if not lines:
                break
            for line in lines:
                if (line.find("sensord") > 0) and (line.find("*DEBUG*") > 0) :
                    print(line)
                if (line.find("deleting OrientationSensorChannel") > 0):
                    success = False
                time.sleep(0.5)

        os.kill(childPid2, signalForKilling)
        self.assert_(success)

if __name__ == '__main__':
	sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
	signal.signal(signal.SIGALRM, timeoutHandler)
	signal.alarm(60)
	unittest.main()
