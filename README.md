
# lmeer

## Table of Contents
- [Info](#info)
- [Description](#description)
- [Config](#config)
- [Usage](#usage)
    - [Integration](#integration)
    - [Implementation](#implementation)
- [Compile](#compile)
- [Install](#install)
- [Uninstall](#uninstall)
- [Contact](#contact)


## Info

- **Name:**         lmeer
- **Author:**       Erfan Shoara
- **Version:**      1.0
- **Release Date:** Apr 25, 2024
- **Repository:**   [link to github
  repo](https://github.com/erfanshoara/lmeer.git)
- **Language:**     C
- **Platform:**     Linux


## Description

it's error utility for C based apps - error trace

<!-- TODO: -->


## Config

right now there isn't any config file for it.


## Usage

### Integration

after installation can include the library to your application:
```c
#include <lmeer.h>
```
and for compilation need to add the installed library based on installation
modes:

- static library

if installed lmeer statically, then need to provide the archive *(liblmeer.a)*
file during compilation of your application. e.g. using GCC:
```bash
gcc -c foo.c
gcc -o foo foo.o liblmeer.a
```

or if the library is installed where the linker can find it, can use:
```bash
gcc -o foo foo.o -llmeer
```

> NOTE; from GCC(1):
> If both static and shared libraries are found, the linker gives preference to
> linking with the shared library unless the -static option is used. 


- shared library

if installed lmeer dynamically, then need to provide the shared object
*(liblmeer.so)* file during compilation of your application. e.g. using GCC:
```bash
gcc -o foo foo.c liblmeer.so
```

or if the library is installed where the linker can find it, can use:
```bash
gcc -o foo foo.c -llmeer
```

to run the application successfully, the shared object must be in a location
that dynamic linker (ld-linux) can find it. <q>See ld.so(8) for more info</q>


### Implementation

<!-- TODO: -->


## Install

use *make* to compile and/or install lmeer either statically or dynamically.
No change should be made in Makefile itself, rather the config.mk can be used
to change the setup for compilation/installation.
After making the desirable changes in config.mk, run the following to compile
the code and generate the library file:
```bash
make clean all
```

and if you want to install as well:
```bash
make clean install
```

> Note; based on your access level and make configuration you might need super
> user permission.


## Uninstall

To uninstall lmeer, the config.mk file should have the same setup as when it
was installed, then run the following:
```bash
make uninstall
```


## Contact

> **Email:** erfan@shoara.net

