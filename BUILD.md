this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# Building Stefan Gebauer's S.M.A.R.T. monitor/analyzer

Stefan Gebauer=ex-student with TU Berlin matriculation number 361095

## General

This affects all ([Graphical User Interface](
http://en.wikipedia.org/wiki/Graphical_user_interface), [service](
http://en.wikipedia.org/wiki/Service), ...) [builds](
http://en.wikipedia.org/wiki/Software_build).

### Building Prerequisites

* [CMake](https://cmake.org/download) for creating makefile / IDE project files
* [C++](https://en.wikipedia.org/wiki/C%2B%2B) [compiler](
   http://en.wikipedia.org/wiki/Compiler) (source code is able to build for
   example with "g++" 7.5.0)
* "[common_sourcecode](https://www.github.com/st-gb/common_sourcecode)" git
repository from Stefan Gebauer(TU Berlin matriculation number 361095)

    Place this repository into the same file system directory level as _this_
    repository for ease of use.:

    ```
        top-level directory
                / \
    SMARTmonitor   common_sourcecode
    ```

### Invocation

1. Change to _repository root file system directory_/src" where 
[CMakeLists.txt](https://cmake.org/cmake/help/latest/guide/tutorial/index.html#a-basic-starting-point-step-1)
resides.

2. either

    * call [CMake](https://cmake.org) directly:

      command line:

      ```
      cmake -G _build_system_generator_ _[...]_
      ```
      
      For available build system generators see output of command line:

      ```
      cmake -G
      ```

    * call the build scripts in _repository root file system directory_/create" 

### Additional options

Additional options can be passed either to CMake command line directly or to the
script file as parameter for "AdditionalCMakeArgs".

If passed to the script for the "AdditionalCMakeOpts" parameter, multiple
options must be enclosed in brackets:

"_option1_=_value1_ _option2_=_value2_" 

#### Specify build type

Pass "-DCMAKE\_BUILD\_TYPE=_type_" to [CMake](https://cmake.org) command line.

The
[build type](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)
can be 1 of

* Debug
* Release
* RelWithDebInfo
* MinSizeRel

## Create Debian package

Pass "-Dcr8DebPkg=TRUE" to [CMake](https://cmake.org) command line.
Call "cpack" afterwards.

## Preparation Examples

### Many/all build targets/executables

This affects (nearly) all targets(service/GUI, ...).

####  Linux

- Debian-/APT-based

  command line:

  ```
  apt-get install g++
  apt-get install cmake
  ```

- g++: GNU C++ compiler

### Direct Access To S.M.A.R.T.

This is essentially needed by server/service/daemon but may also be desired for
the User Interface (wxGUI, command line etc.).

#### Linux

  - Debian-/APT-based

    The "libatasmart" package is needed.

    Command line to install it:

    ```
    apt-get install libatasmart4
    apt-get install libatasmart-dev
    ```

- "libatasmart4" for library (.so, .a) files(?) (to link the executable to)
- "libatasmart-dev" is for header files (libatasmart.h etc.)

### [wxWidgets](http://www.wxwidgets.org/downloads)(tested with version 3) GUI

#### Operating System-independent Prerequisites

For building the wxWidgets GUI for both operating systems "Microsoft Windows"
and "Linux" this software is additionally needed:

-  wxWidgets headers (for compiling)
-  wxWidgets libraries (for linking to and for running the target/executable)
  
  needed wxWidgets libraries:

  - core
  - base
  - adv (for User Interface (control elements) )

#### Linux operating system

see [building the wxWidgets GUI for Linux](build/build_Linux_wxGUI.md)

#### Microsoft Windows operating system

See [building the wxWidgets GUI for Microsoft Windows](
build/build_Windows_wxGUI.md)

### [Makefiles](#makefile)

if the CMake build system generator created a Makefile:

  command line (expects file "Makefile" in current working directory and builds
  there):

  ```
  make -j _number of max. concurrent jobs_
  ```

  argument right of "-j" should be <= _number of CPU cores_

## Authors

[Stefan Gebauer, TU Berlin Computer Science Master](https://github.com/st-gb)

## License

Copyright by Stefan Gebauer(TU Berlin matriculation number 361095), no
commercial use without grant