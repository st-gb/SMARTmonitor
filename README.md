this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# S.M.A.R.T. monitor

A tool to monitor (critical) [S.M.A.R.T.]
(https://en.wikipedia.org/wiki/S.M.A.R.T.) values locally or via BSD sockets.
Values are retrieved periodically in an inverval.


## Building Prerequisites

general:

* [CMake](https://cmake.org/download) for creating makefile / IDE project files
* C++ compiler (source code is able to build with "g++" 7.5.0)
* ["common_sourcecode"](https://www.github.com/st-gb/common_sourcecode) git
repository from Stefan Gebauer

  Place this into the same directory level as this repository for ease of use.:

  ```
        top-level directory
              / \
  SMARTmonitor   common_sourcecode
  ```
* Linux for direct access to S.M.A.R.T.:
  - libatasmart-dev

### wxWidgets GUI:

- [wxWidgets](http://www.wxwidgets.org/downloads) (tested with version 3)
  
  needed libraries:
  - base
  - adv (GTK etc.)

## Preparation Examples

### General
####  Linux

- Debian-/APT-based

  command line:

  ```
  apt-get install g++
  apt-get install cmake
  ```

### Direct Access To S.M.A.R.T.

This is essentially needed by server/service/daemon but may also be desired for
the User Interface (wxGUI, command line etc.).

#### Linux

- Debian-/APT-based

  command line:

  ```
  apt-get install libatasmart-dev
  ```

### wxWidgets GUI

#### Linux

- Debian Linux-/APT-based:
  - command line:

    ```
    apt-get install libwxbase3.0-dev"
    apt-get install libwxgtk3.0-dev"
    ```
    ([...]gtk[...] is "adv" library for Linux/GTK)


## Tests

### minClient

BSD sockets S.M.A.R.T. client with minimal dependencies on the source code.

### minSMARTmonClientDerived

BSD sockets S.M.A.R.T. client derived from class "SMARTmonitorClient".

### readAllSupportedSMART_IDsTime

Measures time to read all S.M.A.R.T. attributes supported by a certain drive. 

## Authors

[Stefan Gebauer, M.Sc. Comp. Sc.](https://github.com/st-gb)

## License

Copyright by Stefan Gebauer, no commercial use without grant
