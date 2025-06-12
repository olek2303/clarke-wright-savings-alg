# Clarke Wright Savings Algorithm

[![CodeQL](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/codeql.yml/badge.svg)](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/codeql.yml)
[![Doxygen Pages](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/doxygen-pages.yml/badge.svg)](https://github.com/olek2303/clarke-wright-savings-alg/actions/workflows/doxygen-pages.yml)
[![License](https://img.shields.io/github/license/olek2303/clarke-wright-savings-alg)](/LICENSE)

![Logo](assets/logo.png)

This repository implements the Clarke-Wright Savings algorithm for a purpose of finding optimal routes through a random set of vertices in 2D space.

## Table of Contents

* [Implementation Details](#implementation-details)
* [Usage](#usage)
* [Results](#results)
* [Documentation](#documentation)
* [License](#license)

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

The solver outputs best routes and plots them onto the original graph. For example:

> Solving with Clarke-Wright based heuristic...
Start depot idx: 0
End depot idx: 49
Number of waypoints: 48
Best route #1: 0 -> 5 -> 3 -> 1 -> 2 -> 4 -> 10 -> 12 -> 15 -> 18 -> 21 -> 19 -> 23 -> 32 -> 34 -> 38 -> 47 -> 46 -> 41 -> 45 -> 8 -> 7 -> 6 -> 13 -> 17 -> 31 -> 35 -> 42 -> 29 -> 16 -> 25 -> 27 -> 33 -> 37 -> 14 -> 26 -> 30 -> 39 -> 40 -> 48 -> 49
Total distance of best route #1: 285.222
Best route #2: 0 -> 5 -> 3 -> 1 -> 2 -> 4 -> 10 -> 12 -> 15 -> 18 -> 21 -> 19 -> 23 -> 32 -> 34 -> 38 -> 47 -> 46 -> 41 -> 45 -> 8 -> 7 -> 6 -> 13 -> 17 -> 31 -> 35 -> 42 -> 29 -> 16 -> 25 -> 27 -> 33 -> 37 -> 14 -> 26 -> 30 -> 39 -> 40 -> 48 -> 49
Total distance of best route #2: 285.222
Best route #3: 0 -> 5 -> 3 -> 1 -> 2 -> 4 -> 10 -> 12 -> 15 -> 18 -> 21 -> 19 -> 23 -> 32 -> 34 -> 38 -> 47 -> 46 -> 41 -> 45 -> 8 -> 7 -> 6 -> 13 -> 17 -> 31 -> 35 -> 42 -> 29 -> 16 -> 25 -> 27 -> 33 -> 37 -> 14 -> 26 -> 30 -> 39 -> 40 -> 48 -> 49
Total distance of best route #3: 285.222
SVG visualization written to graph_with_route.svg
SVG visualization written to graph_clear.svg

<div style="display: flex; gap: 32px; align-items: flex-start;">
  <div>
    <strong>Clear graph for reference:</strong><br>
    <img src="assets/graph_clear.svg" alt="Clear graph example" width="350"/>
  </div>
  <div>
    <strong>Graph with routes plotted:</strong><br>
    <img src="assets/graph_with_route.svg" alt="Graph with rendered route example" width="350"/>
  </div>
</div>

## Documentation

Documentation is auto-generated from docstring via Doxygen and hosted via GitHub Pages:

[View documentation](https://olek2303.github.io/clarke-wright-savings-alg)

To generate locally:

```powershell
doxygen
```

## License

This project is licensed under the [MIT License](LICENSE).
