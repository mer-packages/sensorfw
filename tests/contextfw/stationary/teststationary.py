#!/usr/bin/env python
##
## Copyright (C) 2009-2010 Nokia Corporation
##
## Contact: Lihan Guo <ext-lihan.4.guo@nokia.com>
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

import sys
import unittest
import os
import commands
import string
from string import Template

import time
import signal
import math

from ContextKit.cltool import CLTool

def timeoutHandler(signum, frame):
    raise Exception('Tests have been running for too long')

class Orientation(unittest.TestCase):

    def setUp(self):
        self.fpath = "/tmp/fakedsensors/accelerometer"
        self.datafaker = "/usr/bin/datafaker"
        self.context_client_stable = CLTool("context-listen", "Position.Stable")

    def tearDown(self):
        self.context_client_stable.atexit()

    def testStationary(self):

        index = 0
        # Verify that buffer of similar samples results in stable = true
        while index < 62:
             os.system("echo  -36 -90 953 | " + self.datafaker + " " + self.fpath)
             index = index + 1
             time.sleep(1.1)

        time.sleep(2)
        self.assert_(self.context_client_stable.expect('Position.Stable = bool:true'))

        # Verify that two samples are enough to set stable = false
        os.system("echo  -500 -100 -300 | " + self.datafaker + " " + self.fpath)
        time.sleep(2)
        os.system("echo  -500 -100 -300 | " + self.datafaker + " " + self.fpath)
        time.sleep(2)
        self.assert_(self.context_client_stable.expect('Position.Stable = bool:false'))

        # ...and that we return when those two samples disappear from the buffer
        while index > 0:
            os.system("echo  -500 -100 -300 | " + self.datafaker + " " + self.fpath)
            index = index - 1
            time.sleep(1.1)

        time.sleep(2)
        self.assert_(self.context_client_stable.expect('Position.Stable = bool:true'))

        # Go back to stable = false
        os.system("echo -36 -90 953 | " + self.datafaker + " " + self.fpath)
        time.sleep(2)
        os.system("echo -36 -90 953 | " + self.datafaker + " " + self.fpath)
        time.sleep(2)
        self.assert_(self.context_client_stable.expect('Position.Stable = bool:false'))

        # Verify that timer works
        #Todo: Here should find the configuration file according to device type
        timeout = int(os.popen("cat `ls /etc/sensorfw/sensord.conf.d/* /etc/sensorfw/sensord-ncdk.conf` | grep stability_timeout | head -n1 | cut -f2 -d=", "r").read())
        if (timeout <= 0):
            # Timeout not set in config, 60s is the default
            timeout = 60

        time.sleep(timeout)
        self.assert_(self.context_client_stable.expect('Position.Stable = bool:true'))


if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(600)
    unittest.main()
