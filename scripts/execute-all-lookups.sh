#!/bin/bash

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

# Perform the experimental evaluation performed by Huetter et al. (2022).

mkdir -p results/data/

python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/arxiv/arxiv.bracket -o results/data/arxiv -t 3 5 9 13 3 5 10 15 3 6 11 17 -q 5 2 127 -a 2
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/cards/cards.bracket -o results/data/cards -t 5 10 19 29 7 14 27 40 9 18 35 53 -q 15 423 89 -a 4
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/clothing/clothing.bracket -o results/data/clothing -t 2 3 6 9 2 3 6 9 2 3 6 9 -q 1 2 3 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/dblp/dblp.bracket -o results/data/dblp -t 1 2 4 5 2 3 6 9 2 4 7 10 -q 2 9 1 -a 2
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/denf/denf.bracket -o results/data/denf -t 3 5 10 15 3 5 10 15 3 6 11 16 -q 1 3 7 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/device/device.bracket -o results/data/device -t 7 13 25 37 11 22 44 66 21 41 82 123 -q 122 278 77 -a 5
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/face/face.bracket -o results/data/face -t 2 3 6 9 3 6 11 16 5 10 19 29 -q 2 1 14 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/fenf/fenf.bracket -o results/data/fenf -t 3 5 10 15 3 5 10 15 3 5 10 15 -q 23 51 52 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/movies/movies.bracket -o results/data/movies -t 2 3 5 7 2 3 5 7 2 3 5 7 -q 1 3 4 -a 4
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/nasa/nasa.bracket -o results/data/nasa -t 2 3 6 9 2 3 6 9 2 3 6 9 -q 1 2 3 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/nba/nba.bracket -o results/data/nba -t 45 90 180 270 50 99 197 295 52 103 206 309 -q 2 4 15979 -a 2
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/reads/reads.bracket -o results/data/reads -t 1 2 3 4 1 2 3 4 1 2 3 4 -q 1 2 3 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/recipes/recipes.bracket -o results/data/recipes -t 2 3 5 7 2 3 5 8 2 3 6 9 -q 10 15 2 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/reddit/reddit.bracket -o results/data/reddit -t 13 25 50 74 14 27 54 81 14 28 55 83 -q 24 9 2 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/schema/schema.bracket -o results/data/schema -t 2 4 7 10 3 6 12 18 8 16 31 47 -q 168 113 91 -a 4
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/smsen/smsen.bracket -o results/data/smsen -t 5 9 17 25 5 9 17 25 5 9 17 25 -q 1 2 3 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/smszh/smszh.bracket -o results/data/smszh -t 5 9 17 25 5 9 17 25 5 9 17 25 -q 1 2 3 -a 0
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/spotify/spotify.bracket -o results/data/spotify -t 258 515 1029 1543 258 515 1029 1543 258 515 1029 1543 -q 3 6 7 -a 5
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/standev/standev.bracket -o results/data/standev -t 156 311 621 931 261 522 1043 1565 318 635 1270 1905 -q 19 12 29 -a 4
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/stantrain/stantrain.bracket -o results/data/stantrain -t 75 149 297 445 111 222 444 666 167 333 666 999 -q 136 387 258 -a 4
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/twitter2/twitter2.bracket -o results/data/twitter2 -t 9 17 34 51 10 19 37 56 11 21 42 62 -q 16 156 14 -a 2
python3 scripts/execute-lookup.py -e build/exp-lookup -i external/jedi-datasets/input-data/virus/virus.bracket -o results/data/virus -t 1 2 4 6 1 2 4 6 1 2 4 6 -q 1 2 3 -a 0