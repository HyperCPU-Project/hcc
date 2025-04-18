<img alt="hcc" src="assets/logo.png">

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
- Output (qproc backend with some formatting)
```assembly
main:
    push bp
    mov bp sp

    movi r1 69
    movi r2 3
    div r1 r2

    movi r3 3
    mul r1 r3

    mov r0 r1

    mov sp bp
    pop bp
    pop ip
```

## The hcc executable
It's _kinda_ large (2 Mb or 1.9MiB) because C++, but whatever.

Here's the arguments you can provide to it:

- ```--help``` or ```-h```

Prints a help message containing the usage of HCC
- ```-o <filename>```

Sets an output filename for the generated assembly
- ```--backend <qproc/hypercpu>```

Sets a compilation backend (target)
- ```--ast```

Prints an abstract syntax tree when the parsing is done
