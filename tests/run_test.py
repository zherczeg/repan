#!/usr/bin/env python

#    Regex Pattern Analyzer
#
#    Copyright Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice, this list of
#      conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright notice, this list
#      of conditions and the following disclaimer in the documentation and/or other materials
#      provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL THE COPYRIGHT HOLDER(S) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os
import subprocess
import sys

verbose = False
if len(sys.argv) >= 2 and sys.argv[1] == "-v":
    verbose = True

passed = 0
failed = 0
base = "."
base_len = len(base) + 1

red = "\033[31m"
green = "\033[32m"
default = "\033[0m"

def do_test(path):
    global passed
    global failed
    global base

    for test_name in os.listdir(path):
        test_name = os.path.join(path, test_name)

        if os.path.isdir(test_name):
            do_test(test_name);
            continue

        if not test_name.endswith(".txt") or test_name.endswith("_expected.txt") or path == base:
            continue

        with open("current.txt", "w") as outfile:
            test_retval = subprocess.call(["timeout", "20", "../out/repan-test", test_name], stdout=outfile);

        expected = test_name[0:-4] + "_expected.txt"
        with open("/dev/null", "w") as outfile:
            diff_retval = subprocess.call(["diff", "-q", expected, "current.txt"], stdout=outfile)

        if test_retval == 0 and diff_retval == 0:
            passed += 1
        else:
            print("%sTest %s failed.%s" % (red, test_name[base_len:], default))
            failed += 1
            if verbose:
                subprocess.call(["diff", "-Nu", expected, "current.txt"])

do_test(base)
os.remove("current.txt")

if failed > 0:
    print("\n%sNumber of tests passed: %d%s" % (green, passed, default))
    print("%sNumber of tests failed: %d%s" % (red, failed, default))
else:
    print("%sAll tests passed (%d).%s" % (green, passed, default))

if failed > 0:
    sys.exit(1)
