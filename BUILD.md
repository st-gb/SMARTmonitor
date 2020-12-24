this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# Building S.M.A.R.T. monitor

A tool to monitor raw values for (critical)
[S.M.A.R.T.](https://en.wikipedia.org/wiki/S.M.A.R.T.) parameters locally or
via [BSD sockets](https://en.wikipedia.org/wiki/Berkeley_sockets).
S.M.A.R.T. values are retrieved periodically in an inverval.

Some units (Power-On Time, Total Data Written, ...) for some
[S.M.A.R.T. attributes](https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes)
are tried to be determined.

## general

1. Change to _repository_root_/src" directory where 
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

    * call the build scripts in _repository_root_/create" directory

## Build Examples

### Direct Access To S.M.A.R.T.

This is essentially needed by server/service/daemon but may also be desired for
the User Interface (wxGUI, command line etc.).

#### Linux

- Debian-/APT-based

  The libatasmart package is needed.

  Command line to install it:

  ```
  apt-get install libatasmart4
  ```

### wxWidgets GUI

#### Linux

- Debian Linux-/APT-based:
  - command line to install [wxWidgets](https://www.wxwidgets.org/) runtime
    libraries:

    ```
    apt-get install libwxbase3.0-0v5
    apt-get install libwxgtk3.0-0v5
    ```

    ([...]gtk[...] is "adv" library for Linux/GTK)

  - command line to build (assuming working directory is _repository_root_):

    ```
    cd src
    ../create/create_wxGUI_Linux_debug.sh "Unix Makefiles"
    make -j `nproc`
    ```

### Makefiles

if the CMake build system generator created a Makefile:

  command line (expects file "Makefile" in current working directory and builds
  there):

  ```
  make -j >number of max. concurrent jobs<
  ```

## Authors

[Stefan Gebauer, M.Sc. Comp. Sc.](https://github.com/st-gb)

## License

Copyright by Stefan Gebauer, no commercial use without grant
