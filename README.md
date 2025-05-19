<img alt="hcc" src="images/logo.png">

# Hyper C Compiler: hcc
Multi-backend K&R style C compiler

> [!NOTE]
> This compiler can't compile for architectures like x86, x86_64, arm. This compiler is intended for use with HyperCPU and qproc

HCC is a C compiler, made for HyperCPU and qproc, it's very extensible and it's easy to create backends for it

HCC uses bison, flex, fmt, altough it may not be very optimized in case of codegen, it's a good choice to develop software for above referenced processors

Here's how HCC generated assembly can look like:
- Input
```c
int main(){
    return (69 / 3) * 3;
}
```
- Output (hypercpu backend with some formatting)
```assembly
.attr(entry) hcc_start_main:
	mov xsp, 0xfff;
	mov xbp, 0xfff;
	call main;
	hlt;

main:
	push xbp;
	mov xbp, xsp;

	mov x1, 0u69;
	mov x0, x1;

	mov xsp, xbp;
	pop xbp;
	pop xip;
```

## The hcc executable
It's _kinda_ large (2 Mb or 1.9MiB) because C++, but whatever.

Here's the arguments you can provide to it:

- ```--help``` or ```-h``` — Prints a help message containing the usage of HCC
- ```-o <filename>``` — Sets an output filename for the generated assembly
- ```--backend <qproc/hypercpu>``` — Sets a compilation backend (target)
- ```--ast``` — Prints an abstract syntax tree when the parsing is done

## CMake Build Options
- HCC_NO_TESTS — If set to 1, it will not build tests

## System requirements?
HCC has been tested on these systems:
- Android 10 (armv7, Clang 20.1.2)
- Android (aarch64, Clang 20.1.2)
- Arch Linux (x86_64, gcc 14.2.1 && clang 19.1.7)

(To be fair, every linux system that has bison, flex, and fmt installed will work)
