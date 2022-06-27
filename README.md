# Level of Detail Exploration of Electronic Transition Ensembles using Hierarchical clustering

This repository contains code for the visual analysis pipeline to explore ensembles of electronic transitions (paper presented at EuroVis 2022).
It is built upon the [Inviwo visualization framework](https://inviwo.org/). To use it you will first need to checkout the [Inviwo github repository](https://github.com/inviwo/inviwo) and follow the [Inviwo build instructions](https://inviwo.org/manual-gettingstarted-build.html).
In CMake, add the path to the 'modules' folder in this repository to IVW_EXTERNAL_MODULES.

Python libraries needed:
- scikit learn
- scipy (>= 1.6.0)
- matplotlib

Some processors also require the [ElectronicTransitionsLOD repository](https://github.com/tbmasood/ElectronicTransitionsLOD).

```
 @article{thygesen2022level,
  title={Level of Detail Exploration of Electronic Transition Ensembles using Hierarchical Clustering},
  author={Thygesen, Signe Sidwall and Masood, Talha Bin and Linares, Mathieu and Natarajan, Vijay and Hotz, Ingrid},
  year={2022}
}
```
