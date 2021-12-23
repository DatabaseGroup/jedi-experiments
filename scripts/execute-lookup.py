#!/usr/bin/env python3

# The MIT License (MIT)
# Copyright (c) 2021 Thomas Huetter.
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""lookup-exp.py: Execute similarity lookup queries for given parameters."""

import sys
import math
from argparse import ArgumentParser
import subprocess

def main(argv):
    # Read command line arguments.
    parser = ArgumentParser(description='Input parameters for experimental \
        evaluation')
    parser.add_argument('-e', '--executable', type=str,
        required=True, help='<Required> Filename/-path where the executable \
        is stored.')
    parser.add_argument('-i', '--inputfiles', nargs='+', type=str, default=[],
        required=True, help='<Required> Filename/-path where the experimental \
        results are stored.')
    parser.add_argument('-o', '--outputfiles', nargs='+', type=str, default=[],
        required=True, help='<Required> Filename/-path where the results will \
        be stored.')
    parser.add_argument('-t','--thresholds', nargs='+', type=int, default=[],
        required=True, help='<Required> List of thresholds.')
    parser.add_argument('-q','--querytrees', nargs='+', type=int, default=[],
        required=True, help='<Required> Indices of three query trees.')
    parser.add_argument('-a','--algorithms', type=int, default=0,
        required=True, help='<Required> Specify the executed algorithms.')
    args = parser.parse_args()

    cargs = ""
    for fileidx in range(len(args.inputfiles)):
        for treeidx in range(len(args.querytrees)):
            for thresholdidx in range((treeidx*4), (treeidx*4)+4):
                try:
                    cargs = [args.executable, args.inputfiles[fileidx], 
                        str(args.thresholds[thresholdidx]), 
                        args.outputfiles[fileidx], str(args.querytrees[treeidx]), 
                        str(args.algorithms)]
                    print(cargs)
                    popen = subprocess.Popen(cargs, stdout=subprocess.PIPE)
                    popen.wait(timeout=86400) # Timeout in seconds.
                    output = popen.stdout.read()
                except:
                    print("Execution failed.")
                    continue


if __name__ == '__main__':
    main(sys.argv)
