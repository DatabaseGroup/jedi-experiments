# jedi-experiments

This repository contains the experimental evaluation of JSON similarity lookups that are published in the following publication:

```
@inproceedings{huetter2021jedi,
  title        = {{JEDI}: These aren't the {JSON} documents you're looking for...},
  author       = {H{\"u}tter, Thomas and Augsten, Nikolaus and Christoph M, Kirsch and Michael J, Carey and Li, Chen},
  year         = 2022,
  booktitle    = {Proceedings of the 2022 International Conference on Management of Data}
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

## Building the project

Get the external libraries common-code and tree-similarity used for time measurements and algorithms.
```
mkdir -p external
cd external
git clone https://frosch.cosy.sbg.ac.at/wmann/common-code.git
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

To perform the experiments and plot the results by Huetter et al. (2022), the following steps have to be performed. First, the experimental data needs to be fetched.
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

## Reproduce the experiments within a Docker container

The `Dockerfile` in the root directory of this repository allows to reproduce the entire experimental evaluation within a Docker a container. To do so, please execute the following commands:
```
mkdir -p results
docker build --no-cache -t jedi-experiments .
docker run -d -ti --name jedi-exp --mount type=bind,source="$(pwd)"/results,target=/usr/src/app/jedi-experiments/results jedi-experiments
```

This command will persistently store the experimental results in the mounted directory (here, in the created directory `results`). In case that the data should not be persistently stored, remove the `--mount type=bind,source="$(pwd)"/results,target=/usr/src/app/jedi-experiments/results` argument.