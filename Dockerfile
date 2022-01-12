# syntax=docker/dockerfile:1

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

# Description: Experimental evaluation performed by Huetter et al. (2022) in
# "JEDI: These aren't the JSON documents you're looking for...". The
# code is fetched from https://github.com/DatabaseGroup/jedi-experiments and
# the data is fetched from https://github.com/DatabaseGroup/jedi-datasets.

# Execution: First, create a directory 'results' where the container
# persistently stores the results. Next, build the image. Finally, run the
# experiments in the docker container. To do so, execute the following
# instructions:
#  - Prepare: mkdir results
#  - Build: docker build --no-cache -t jedi-experiments .
#  - Run: docker run -ti --name jedi-exp --mount \
#         type=bind,source="$(pwd)"/results, \
#         target=/usr/src/app/jedi-experiments/results jedi-experiments

FROM debian:10

# Set working directory
WORKDIR /usr/src/app

# LABEL about the custom image
LABEL maintainer="thomas.huetter@plus.ac.at"
LABEL version="1.0"
LABEL description="This is custom Docker Image for the reproducibility of the \
experimental evaluation published in the JEDI paper by Huetter et al. at \
SIGMOD 2022."

# Disable Prompt During Packages Installation
ARG DEBIAN_FRONTEND=noninteractive

# Update Ubuntu Software repository
RUN apt update

# Install python 3.7
RUN apt install software-properties-common -y
RUN add-apt-repository ppa:deadsnakes/ppa
RUN apt install python3.7 -y
RUN apt install cmake -y
RUN apt install gcc -y

# Add 3.7 to the available alternatives
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.7 1

# Set python3.7 as the default python
RUN update-alternatives --set python /usr/bin/python3.7

# Install pip
RUN apt install python3-pip -y
RUN python -m pip install --upgrade pip

# Install git
RUN apt install gcc git git-lfs -y

# Clean apt
RUN apt clean

# Install python libraries using pip
RUN pip install --no-cache-dir --upgrade pip && \
    pip install --no-cache-dir matplotlib pandas

# Clone repository with experimental framework
RUN git clone -b v1.0.0 https://github.com/DatabaseGroup/jedi-experiments.git
WORKDIR ./jedi-experiments

# Setup experiment.
RUN chmod 0755 scripts/reproduce-experiment.sh
CMD scripts/reproduce-experiment.sh
