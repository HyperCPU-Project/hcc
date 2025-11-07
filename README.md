<div align="center">
    <img alt="Hyper C Compiler" src="images/logo.png">
    <h1>Hyper C Compiler</h1>
    <p>
        <a href="https://github.com/HyperCPU-Project/hcc/issues"><img src="https://img.shields.io/github/issues/HyperCPU-Project/hcc" alt="GitHub opened issues"></a>
        <img src="https://img.shields.io/badge/status-in_development-red" alt="Status">
        <img src="https://img.shields.io/github/license/HyperCPU-Project/hcc" alt="License">
        <img src="https://img.shields.io/github/stars/HyperCPU-Project/hcc?color=lime" alt="Stars">
    </p>
</div>

Hyper C Compiler (hcc) is a C compiler developed for HyperCPU and qproc. It's very extensible and allows easily create backends for it.

Hyper C Compiler uses `bison`, `flex` and `fmt` libraries. Although it may be not very optimized in code generation, it's a good choice to develop software for above referenced processors.

> [!WARNING]
> The compiler is intended for use with HyperCPU and qproc only. It can't compile for architectures like x86_32, x86_64, ARM64, etc.

Here's how hcc generated assembly can look like:

- Input (C)
```c
int main(){
    return (69 / 3) * 3;
}
```

- Output (hASM; HyperCPU backend with constant folding, DCE and fp omission)

```assembly
main:
mov x0, 0u69;
ret;
```

### Installation

>[!WARNING]
> The project supports GNU/Linux and Windows (msys2) systems only. On GNU/Linux, it is known to be successfully built on `x86_32`, `x86_64`, `ARMv7` and `ARMv8` architectures.
>
> BSD* support is not tested, but it could possibly be built under this system

#### Binaries

Pre-compiled binaries are currently not available. Sorry.

#### Building from sources

Building Hyper C Compiler requires these dependencies to be installed:

* GCC `>=12` or Clang `>=14`;
* GNU make, CMake `>=3.25`;
* `bison`, `flex` and `fmt` libraries.

After installing dependencies build the project by executing these commands:

```bash
git clone --recursive https://github.com/HyperCPU-Project/hcc # Clone the repository.
cd hcc
cmake -S . -B build # Generate build files.
cd build
make
```

Besides `hcc` executable file, tests also will be built. If you are not interested in tests, specify `HCC_NO_TESTS=1` CMake build option.

### Usage

`hcc` executable file is _kinda_ large (*2 MB / 1.9MiB*) because of C++, but whatever.

```bash
$ ./hcc [-h | --help] [-o FILENAME] [--backend BACKEND] [--ast] [-fOPTIMIZATION] [-fno-OPTIMIZATION] source
```

* `source` — source code file to be compiled.
* `-h`, `--help` — display help message and exit.
- `-o FILENAME` — specify output binary file name.
- `--backend BACKEND` — specify compilation backend (target). Available values are: `qproc`, `hypercpu`.
- `--ast` — print an abstract syntax tree when the parsing is done.
- `-fOPTIMIZATION` — enable optimization.
- ```-fno-OPTIMIZATION``` — disables optimization.

### Authors

Hyper C Compiler is brought to you by these wonderful people:

* **[aceinetx](https://github.com/aceinetx)** — project founder and lead developer.
* **[Ivan Movchan](https://github.com/ivan-movchan)** — project artist and just a contributor.

### License

Hyper C Compiler is free software released under the terms of the [GNU General Public License v3.0](LICENSE).
