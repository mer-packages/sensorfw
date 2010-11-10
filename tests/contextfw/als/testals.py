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
import string
import time
import signal
from ContextKit.cltool import CLTool


def timeoutHandler(signum, frame):
    raise Exception('Tests have been running for too long')

class Orientation(unittest.TestCase):

    def setUp(self):
        self.fpath = "/tmp/fakedsensors/als"
        self.context_client_dark = CLTool('context-listen', 'Environment.IsDark')
        self.context_client_bright = CLTool('context-listen', 'Environment.IsBright')

    def tearDown(self):
        self.context_client_dark.atexit()
        self.context_client_bright.atexit()

    def testIsDark(self):
        # Normal
        os.system("echo -en '\x94' >" + self.fpath) # 148
        self.assert_(self.context_client_dark.expect('Environment.IsDark = bool:false'))

        # Dark
        os.system("echo -en '\x09' >" + self.fpath) # 9
        self.assert_(self.context_client_dark.expect('Environment.IsDark = bool:true'))

        # Bright
        os.system("echo -en '\x33\x01' >" + self.fpath) # 307
        self.assert_(self.context_client_dark.expect('Environment.IsDark = bool:false'))

    def testIsBright(self):
        # Normal
        os.system("echo -en '\x94' >" + self.fpath) # 148
        self.assert_(self.context_client_bright.expect('Environment.IsBright = bool:false'))

        # Bright
        os.system("echo -en '\x33\x01' >" + self.fpath) # 307
        self.assert_(self.context_client_bright.expect('Environment.IsBright = bool:true'))

        # Dark
        os.system("echo -en '\x09' >" + self.fpath) # 9
        self.assert_(self.context_client_bright.expect('Environment.IsBright = bool:false'))

if __name__ == '__main__':
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(5)
    unittest.main()
