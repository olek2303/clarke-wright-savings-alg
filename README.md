# Graph Solvers Template

[![CodeQL](https://github.com/milosz275/graph-solvers-template/actions/workflows/codeql.yml/badge.svg)](https://github.com/milosz275/graph-solvers-template/actions/workflows/codeql.yml)
[![Doxygen Pages](https://github.com/milosz275/graph-solvers-template/actions/workflows/doxygen-pages.yml/badge.svg)](https://github.com/milosz275/graph-solvers-template/actions/workflows/doxygen-pages.yml)
[![License](https://img.shields.io/github/license/milosz275/graph-solvers-template)](/LICENSE)

![Logo](assets/logo.png)

Graph solvers template can be used to visualize your graph algorithm using CPLEX on randomly created graph visualized along with the wanted output.

## Table of Contents

* [Overview](#overview)
* [Implementation Details](#implementation-details)
* [Usage](#usage)
* [Testing](#testing)
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
* **Testing**: GoogleTest
* **Documentation**: Doxygen

## Usage

To use this template click `Use this template` and create a new repository or open the template in a codespaces [or use this template here](https://github.com/new?template_name=graph-solvers-template&template_owner=milosz275).

Then make sure to initialize submodules:

```powershell
git submodule update --init --recursive
```

Remember to clone your repo with recursive flag too:

```powershell
git clone --recursive https://github.com/your_username/your_repo_name.git
```

This ensures the `CDT` library is properly initialized.

> [!IMPORTANT]
> Remember is to replace `milosz275` with your username and `graph-solvers-template` with your repository name across all files.

### VSCode / Visual Studio 2022

CPLEX is automatically located if installed in a standard path.

Simply open the folder in VSCode with the CMake extension, choose `x64-release` preset and run.

### Developer PowerShell for VS

Run the following commands to configure and build the project:

```powershell
cmake --preset x64-release -DCPLEX_ROOT="C:/Program Files/IBM/ILOG/CPLEX_Studio2212"
cmake --build --preset x64-release
.\out\build\x64-release\graph-solvers-template\graph-solvers-template.exe
```

> Use Developer PowerShell to ensure `cl.exe` and `CPLEX` are in the environment.

## Testing

Implement your tests under `graph-solvers-template/tests` by following example scheme. IDEs should automatically detect them.

To run tests via CLI:

```powershell
ctest --preset x64-release
```

Or run `test_*.exe` directly from the build folder.

## Documentation

Documentation is auto-generated from docstring via Doxygen and hosted via GitHub Pages:

[View documentation](https://milosz275.github.io/graph-solvers-template)

To generate locally:

```powershell
doxygen
```

## License

This project is licensed under the [MIT License](LICENSE).
