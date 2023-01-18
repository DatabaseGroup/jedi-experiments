[![DOI](https://zenodo.org/badge/442186830.svg)](https://zenodo.org/badge/latestdoi/442186830)

# jedi-experiments

This repository contains the experimental evaluation of JSON similarity lookups that are published in the following publication:

```
@inproceedings{huetter2022jedi,
  author = {H\"{u}tter, Thomas and Augsten, Nikolaus and Kirsch, Christoph M. and Carey, Michael J. and Li, Chen},
  title = {JEDI: These Aren't the JSON Documents You're Looking For?},
  year = {2022},
  isbn = {9781450392495},
  publisher = {Association for Computing Machinery},
  address = {New York, NY, USA},
  url = {https://doi.org/10.1145/3514221.3517850},
  doi = {10.1145/3514221.3517850},
  pages = {1584–1597},
  numpages = {14},
  location = {Philadelphia, PA, USA},
  series = {SIGMOD '22}
}
```

## Software requirements

The following list of software is required to execute the experiments:
  * python 3
  * gcc (or another compiler that supports C++11)
  * cmake
  * git
  * git-lfs
  * matplotlib
  * pandas

## Hardware requirements

Reproducing the experiments requires approximately 64GB RAM. Note that otherwise, the experiments on larger datasets will fail and no results are produced.

## Building the project

Get the external libraries common-code and tree-similarity used for time measurements and algorithms.
```
mkdir -p external
cd external
git clone -b original https://frosch.cosy.sbg.ac.at/code/cpp/timing-statistics.git
git clone -b sigmod2022 https://github.com/DatabaseGroup/tree-similarity.git
cd ..
```

Build the project.
```
mkdir -p build
cd build
cmake ..
make
```

The resulting binary is located at `build/exp-lookup`.

## Reproduce the experiments

In order to perform the experiments and plot the results by Huetter et al. (2022), the following two steps have to be performed. First, the experimental data needs to be fetched and pre-processed.
```
cd external
git clone -b v1.0.0 https://github.com/DatabaseGroup/jedi-datasets.git
cd jedi-datasets
sh scripts/download-prepare.sh
cd ../..
```

At this point, the data and the source code are present. Finally, use the `perform-experiment.sh` script to execute experiments and plot the data.

```
sh scripts/perform-experiment.sh
```

The experimental results and the plots are located in the created `results` directory.

### Reproduce the experiments within a Docker container

The `Dockerfile` in the root directory of this repository allows to reproduce the entire experimental evaluation within a Docker a container. To do so, please execute the following commands:
```
mkdir -p results
docker build --no-cache -t jedi-experiments .
docker run -d -ti --name jedi-exp --mount type=bind,source="$(pwd)"/results,target=/usr/src/app/jedi-experiments/results jedi-experiments
```

This command will persistently store the experimental results in the mounted directory (here, in the created directory `results`). In case that the data should not be persistently stored, remove the `--mount type=bind,source="$(pwd)"/results,target=/usr/src/app/jedi-experiments/results` argument.

Note that the overall experiment approximatelly takes up to **20 hours**.
