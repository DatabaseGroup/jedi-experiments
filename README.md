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

## Building the project

Get the external libraries common-code and tree-similarity used for time measurements and algorithms.
```
mkdir -p external
cd external
git clone https://frosch.cosy.sbg.ac.at/wmann/common-code.git
git clone https://github.com/DatabaseGroup/tree-similarity.git
cd ..
```

Build the project.
```
mkdir -p build
cd build
cmake ..
make
```

Perform the experiments presented by Huetter et al. (2022).
```
cd external
git clone https://github.com/DatabaseGroup/jedi-datasets.git
cd jedi-datasets
sh scripts/download-prepare.sh
cd ../..
sh scripts/execute-all-lookups.sh
```