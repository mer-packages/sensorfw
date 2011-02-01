#!/usr/bin/env python
##
## Copyright (C) 2009-2010 Nokia Corporation
##
## Contact: Jean-Luc Lamadon <jean-luc.lamadon@nokia.com>
##          Matias Muhonen <ext-matias.muhonen@nokia.com>
##          Tapio Rantala <ext-tapio.rantala@nokia.com>
##          Lihan Guo <lihan.guo@digia.com>
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
        self.context_client_edge = CLTool("context-listen", "Screen.TopEdge", "Position.IsFlat")
        self.context_client_cover = CLTool("context-listen", "Screen.IsCovered")

        # Get angle thresholds from config
        landscape_angle = int(os.popen("cat `ls /etc/sensorfw/sensord.conf.d/* /etc/sensorfw/sensord.conf` | grep orientation_threshold_landscape | head -n1 | cut -f2 -d=", "r").read())
        portrait_angle = int(os.popen("cat `ls /etc/sensorfw/sensord.conf.d/* /etc/sensorfw/sensord.conf` | grep orientation_threshold_portrait | head -n1 | cut -f2 -d=", "r").read())

        print("Using thresholds for orientation changes:\n  Landscape: " + str(landscape_angle) + "\n  Portrait: " + str(portrait_angle) + "\n")

        # Create data sets
        self.dataSet = []
        self.expectSet = []

        dataSet_top = []
        dataSet_left = []

        # TopEdge = top (U, U, T, U, T), starting from Unknown
        for angle in [0, portrait_angle-1, portrait_angle+1, portrait_angle-1, 90]:
            dataSet_top.append([0, int(1000*math.cos(math.radians(90-angle))), int(1000*math.cos(math.radians(angle)))])
        self.dataSet += dataSet_top

        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('')
        self.expectSet.append('Screen.TopEdge = QString:"top"')
        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('Position.IsFlat = bool:false')

        # TopEdge = left (U, U, L, U, L)
        for angle in [0, landscape_angle-1, landscape_angle+1, landscape_angle-1, 90]:
            dataSet_left.append([-int(1000*math.cos(math.radians(90-angle))), 0, int(1000*math.cos(math.radians(angle)))])
        self.dataSet += dataSet_left

        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('')
        self.expectSet.append('Screen.TopEdge = QString:"left"')
        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('Position.IsFlat = bool:false')

        # TopEdge = bottom, (U, U, B, U, B)
        for v in dataSet_top[:]:
            u = v[:]
            u[1] = -u[1]
            self.dataSet.append(u)

        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('')
        self.expectSet.append('Screen.TopEdge = QString:"bottom"')
        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('Position.IsFlat = bool:false')

        # TopEdge = right (U, U, R, U, R)
        for v in dataSet_left[:]:
            u = v[:]
            u[0] = -u[0]
            self.dataSet.append(u)

        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('')
        self.expectSet.append('Screen.TopEdge = QString:"right"')
        self.expectSet.append('Position.IsFlat = bool:true')
        self.expectSet.append('Position.IsFlat = bool:false')

        # TopEdge: left -> top -> left (should represent bottom and right well enough)
        for angle in [90, portrait_angle-1, portrait_angle+1, 90-portrait_angle, 0]:
            self.dataSet.append([-int(1000*math.cos(math.radians(angle))), int(1000*math.cos(math.radians(90-angle)

        self.expectSet.append('Screen.TopEdge = QString:"top"')
        self.expectSet.append('Screen.TopEdge = QString:"left"')
        self.expectSet.append('')
        self.expectSet.append('Screen.TopEdge = QString:"top"')
        self.expectSet.append('Screen.TopEdge = QString:"left"')

    def tearDown(self):
        self.context_client_edge.atexit()
        self.context_client_cover.atexit()

    def testOrientation(self):

        # Set the starting position to bottom (0, -1000, 0)
        os.system("echo 0 -1000 0 | " + self.datafaker + " " + self.fpath)

        index = 0
        for v in self.dataSet[:]:
            time.sleep(0.9)
            if self.expectSet[index] != '':
                os.system("echo " + str(v[0]) + " " + str(v[1]) + " " + str(v[2]) + " | " + self.datafaker + " " + self.fpath)
                self.assert_(self.context_client_edge.expect(self.expectSet[index]))
            index += 1

        # Set the starting position
        os.system("echo 0 0 -1000 | " + self.datafaker + " " + self.fpath)
        time.sleep(0.9)

        # On the table
        os.system("echo -36 -90 953 | " + self.datafaker + " " + self.fpath)
        self.assert_(self.context_client_cover.expect('Screen.IsCovered = bool:false'))
        time.sleep(0.9)

        # On the table upside down
        os.system("echo 270 216 -972 | " + self.datafaker + " " + self.fpath)
        self.assert_(self.context_client_cover.expect('Screen.IsCovered = bool:true'))


if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(30)
    unittest.main()
