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

"""plot.py: Print data that is stored in a csv file."""

import os
import sys
import math
import glob
import matplotlib.pyplot as plt
import pandas as pd
from argparse import ArgumentParser


# Linestyles for twelve algorithmic configurations.
linestyles = ['-', '-.'] #, '--', ':']
colors = ['#ca6680', '#78c0dd', '#63a375', '#946592', '#e8a77c', '#4f6ece']
label = ['Pre-Candidates', 'Candidates', 'Result', 'Upper Bound']

def main(argv):
    # Read command line arguments.
    parser = ArgumentParser(description='Input parameters for ploting the \
        results')
    parser.add_argument('-i', '--inputpath', type=str, required=True,
        help='<Required> Filenpath where the experimental results are stored.')
    parser.add_argument('-o', '--outputpath', type=str, required=True,
        help='Filepath where the plot will be stored. Otherwise, the \
         results will be shown in a seperate window.')
    parser.add_argument('-c','--cols', nargs='+',
        help='<Required> Give a list of column names that should be printed.')
    parser.add_argument('-sr', '--skiprows', type=int, default=0,
        help='Skip the first <sr> rows while parsing the input file.')
    parser.add_argument('-t', '--title', type=str,
        help='Print a title.')
    parser.add_argument('-xl', '--xlabel', type=str, default="X",
        help='Set label of x-axis.')
    parser.add_argument('-yl', '--ylabel', type=str, default="Y",
        help='Set label of y-axis.')
    parser.add_argument('-ys', '--yscale', type=str,
        help='Set scale of y-axis, e.g., "symlog" for logarithmic scale. Use \
        linear scale if no parameter is given.')
    parser.add_argument('-l', '--legend', action='store_true',
        help='Print legend.')
    args = parser.parse_args()

    result_files = glob.glob(args.inputpath + '*runtime.txt')

    if len(result_files) == 0:
        print("No result files found that matches: '" + str(args.inputpath) + 
            "*runtime.txt'.")

    index = {}
    for fpath in result_files:
        fsplit = os.path.basename(fpath).split('-')
        if fsplit[0] not in index:
            index[fsplit[0]] = {}
        if int(fsplit[2]) not in index[fsplit[0]]:
            index[fsplit[0]][int(fsplit[2])] = {}
        index[fsplit[0]][int(fsplit[2])][int(fsplit[1].split('.')[0])] = fpath
    nr_datasets = len(index)

    # Process the results of all datasets.
    for dataset, value1 in index.items():
        for querytree, value2 in value1.items():
            li = []
            thresholds = []
            collection_size = 0
            pre_candidates = []
            candidates = []
            result = []
            result_ub = []
            for threshold, filepath in sorted(value2.items()):
                try:
                    df = pd.read_csv(filepath, index_col=None, header=0, 
                        skiprows=0)
                    df2 = pd.read_csv(filepath.replace("runtime", "meta"), 
                        index_col=None, header=0, skiprows=0)
                    collection_size = float(df2.iloc[0,0])
                except:
                    print("Skip file: " + filepath)
                    continue
                li.append(df)
                thresholds.append(threshold)

            frame = pd.concat(li, axis=0, ignore_index=True)
            frame = frame.astype(float)
            frame[frame < 0.009] = 0

            width = 0.8
            t = list(map(str, thresholds))

            algorithm = len(list(frame.columns)) - 1 # Fix best version.
            collection = [collection_size] * int(len(thresholds))
            pre_cand = list(map(float, list(frame.iloc[:, algorithm])[0::6]))
            candidates = list(map(float, list(frame.iloc[:, algorithm])[1::6]))
            result = list(map(float, list(frame.iloc[:, algorithm])[4::6]))
            result_ub = list(map(float, list(frame.iloc[:, algorithm])[2::6]))

            plt.figure(figsize=(3, 3))
            plt.bar(t, collection, label = 'Dataset Size', color=colors[4], 
                align='center', width=width)
            plt.bar(t, pre_cand, label = 'Pre-Candidates', color=colors[0], 
                align='center', width=width)
            plt.bar(t, candidates, label = 'Candidates', color=colors[1], 
                align='center', width=width)
            # plt.bar(t, result, label = 'Result', color=colors[2], 
            #   align='center', width=width)
            plt.bar(t, result_ub, label = 'Upper Bound', color=colors[3], 
                align='center', width=width)

            x1,x2,y1,y2 = plt.axis()  
            plt.axis((x1,x2,1,y2))

            # Set user-specific plot parameters, e.g., label and scale of the 
            # axis.
            if args.title:
                plt.title(args.title + " - " + dataset, fontsize=14)
            if args.xlabel:
                plt.xlabel(args.xlabel, fontsize=14)
            if args.ylabel:
                plt.ylabel(args.ylabel, fontsize=14)
            if args.yscale:
                plt.yscale(args.yscale)

            if args.legend:
                # Set legend parameters as specified in the parameters.
                legend = plt.legend(fontsize=8)

            plt.tight_layout()

            # If a name for an output file is given print the plot, otherwise 
            # show it.
            if args.outputpath:
                plt.savefig(args.outputpath + dataset + "-" + str(querytree) + 
                    ".eps", bbox_inches='tight')
            else:
                plt.show()
            plt.clf()


if __name__ == '__main__':
    main(sys.argv)
