# Clarke Wright Savings Algorithm

[![CodeQL](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/codeql.yml/badge.svg)](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/codeql.yml)
[![Doxygen Pages](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/doxygen-pages.yml/badge.svg)](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/doxygen-pages.yml)
[![License](https://img.shields.io/github/license/olek2303/clarke-wright-savings-alg)](/LICENSE)

![Logo](assets/logo.png)

Graph solvers template can be used to visualize your graph algorithm using CPLEX on randomly created graph visualized along with the wanted output.

## Table of Contents

* [Overview](#overview)
* [Implementation Details](#implementation-details)
* [Usage](#usage)
* [Results](#results)
* [Documentation](#documentation)
* [License](#license)

## Overview

This project creates an entry point for implementing a solver for any graph problem using integer programming (IP). IBM CPLEX is connected as the solver backend using CMake. Results are visualized with CDT in SVG format, overlaying the algorithm results.

## Implementation Details

* **Language**: C++20
* **Vertices generation**: Uniform distribution in 2D space
* **Solver**: [IBM CPLEX](https://www.ibm.com/products/ilog-cplex-optimization-studio)
* **LP Library**: CPLEX Concert Technology
* **Visualization**: [CDT](https://github.com/artem-ogre/CDT)
* **Documentation**: Doxygen

## Usage

Make sure to clone the repository with submodules:

```powershell
git clone --recursive https://github.com/olek2303/clarke-wright-savings-alg.git
```

If you've already cloned the repository without `--recursive`, run:

```powershell
git submodule update --init --recursive
```

This ensures the `CDT` library is properly initialized.

### VSCode / Visual Studio 2022

CPLEX is automatically located if installed in a standard path.

Simply open the folder in VSCode with the CMake extension, choose `x64-release` preset and run.

### Developer PowerShell for VS

Run the following commands to configure and build the project:

```powershell
cmake --preset x64-release -DCPLEX_ROOT="C:/Program Files/IBM/ILOG/CPLEX_Studio2212"
cmake --build --preset x64-release
.\out\build\x64-release\clarke-wright-savings-alg\clarke-wright-savings-alg.exe
```

> Use Developer PowerShell to ensure `cl.exe` and `CPLEX` are in the environment

## Results

First, clear graph is rendered:

![Clear graph example](assets/graph_clear.svg)

Then the solution is visualized as paths over the graph:

![Graph with rendered route example](assets/graph_with_route.svg)

## Documentation

Documentation is auto-generated from docstring via Doxygen and hosted via GitHub Pages:

[View documentation](https://olek2303.github.io/clarke-wright-savings-alg)

To generate locally:

```powershell
doxygen
```

## License

This project is licensed under the [MIT License](LICENSE).
