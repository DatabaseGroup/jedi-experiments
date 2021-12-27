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

"""plot-runtime.py: Plots experimental runtime results which are stored in a
csv file. The script is executed as follows: 
python3.6 plot_lookup_runtime_new.py -i ../execution/2021_06_27_data/ -o test2/ 
-ds "../execution/2021_06_27_data/face" -ys "symlog" -yl "Runtime [in ms]" 
-xl "Threshold" --l"""

import os
import sys
import math
import glob
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from argparse import ArgumentParser


# Linestyles for twelve algorithmic configurations.
linestyles = ['-'] #, '-.', '--', ':']
colors = ['#ca6680', '#e8a77c', '#946592', '#4f6ece', '#e8a77c', '#63a375',
            '#ca6680', '#e8a77c', '#946592', '#4f6ece', '#e8a77c', '#78c0dd']
markers = ['.', '.', '.', '.', '.', 'X', '*', 'p', '.', '.', '.', 'D']
markersizes = [0, 0, 0, 0, 0, 15, 20, 20, 30, 0, 0, 15]

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
            values = []
            for threshold, fpath in sorted(value2.items()):
                try:
                    df = pd.read_csv(fpath, index_col=None, header=0,
                        skiprows=0)
                except:
                    print("Skip file: " + fpath)
                    continue
                li.append(df)
                thresholds.append(threshold)

            frame = pd.concat(li, axis=0, ignore_index=True)
            frame = frame.astype(float)
            frame[frame < 0.009] = 0

            algorithms = list(frame.columns)

            # All algorithmic configurations.
            # exclude_all = ['scan_baseline','scan_quickjedi',
            #     'scan_baseline_wang','scan_quickjedi_wang',
            #     'scan_baseline_jofilter','scan_quickjedi_jofilter',
            #     'index_baseline','index_quickjedi','index_baseline_wang',
            #     'index_quickjedi_wang','index_baseline_jofilter',
            #     'index_quickjedi_jofilter']

            # All algorithmic configurations that are considered in the JEDI 
            # paper by Huetter et al. (2022), called "JEDI: These aren't the 
            # JSON documents you're looking for...".
            exclude_algos = ['scan-baseline','scan-quickjedi',
                'scan-baseline-wang','scan-quickjedi-wang',
                'scan-baseline-jofilter','index-quickjedi-wang',
                'index-baseline-jofilter']

            # Extract the data of all included algorithmic configurations.
            for algo in range(len(algorithms)):
                # Set label to algorithmic configuration.
                lab = algorithms[algo]

                # Get results from the according column.
                values.append(list(map(float, list(frame.iloc[:, algo])[5::6])))

                # Exclude algorithmic configurations without results.
                if all(v == 0 for v in values[algo]):
                    lab = "_Hidden label"
                    continue

                # Exclude algorithmic configurations as specified above.
                if lab in exclude_algos:
                    for i in range(len(values[algo])):
                        values[algo][i] = 0
                    lab = "_Hidden label"
                    continue

                # Plot results.
                plt.plot(thresholds, values[algo], linewidth=3, label = lab, 
                    linestyle=linestyles[algo % len(linestyles)], 
                    color=colors[algo % len(colors)], 
                    marker=markers[algo % len(markers)],
                    markersize=markersizes[algo % len(markersizes)])

            # Set user-specific plot parameters, e.g., label and scale of the
            # axis.
            if args.title:
                plt.title(args.title + " - " + dataset, fontsize=14)
            if args.xlabel:
                plt.xlabel(args.xlabel, fontsize=16)
            if args.ylabel:
                plt.ylabel(args.ylabel, fontsize=16)
            if args.yscale:
                plt.yscale(args.yscale)

            if args.legend:
                # Set legend parameters as specified in the parameters.
                legend = plt.legend(fontsize=10)

            plt.tick_params(axis='both', which='major', labelsize=16)
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