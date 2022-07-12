# Level of Detail Exploration of Electronic Transition Ensembles using Hierarchical clustering

This repository contains code for the visual analysis pipeline to explore ensembles of electronic transitions ([paper presented at EuroVis 2022](https://diglib.eg.org/handle/10.1111/cgf14544)).
It is built upon the [Inviwo visualization framework](https://inviwo.org/). To use it you will first need to checkout the [Inviwo github repository](https://github.com/inviwo/inviwo) and follow the [Inviwo build instructions](https://inviwo.org/manual-gettingstarted-build.html).
In CMake, add the path to the 'modules' folder in this repository to IVW_EXTERNAL_MODULES.

Python libraries needed:
- scikit learn
- scipy (>= 1.6.0)
- matplotlib

Some processors also require the [ElectronicTransitionsLOD repository](https://github.com/tbmasood/ElectronicTransitionsLOD) (using Java).

An example workspace is located in the data folder in the MolecularChargeTransitions module, which uses some randomly generated test data. 


BibTeX:
```
@article {10.1111:cgf.14544,
  journal = {Computer Graphics Forum},
  title = {{Level of Detail Exploration of Electronic Transition Ensembles using Hierarchical Clustering}},
  author = {Sidwall Thygesen, Signe and Masood, Talha Bin and Linares, Mathieu and Natarajan, Vijay and Hotz, Ingrid},
  year = {2022},
  publisher = {The Eurographics Association and John Wiley & Sons Ltd.},
  ISSN = {1467-8659},
  DOI = {10.1111/cgf.14544}
}
```
