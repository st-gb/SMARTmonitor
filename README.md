
[comment]: # https://stackoverflow.com/questions/4823468/comments-in-markdown
[comment]: # "With an empty line before the comment."
[comment]: # Line breaks may appear after "(" for a link as this is the (only)
[comment]: # possibility to break the line at 80 characters and avoid spaces in
[comment]: # links.

this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# S.M.A.R.T. Values Monitor/Processor

## Capabilities

### Monitoring Directly/Locally or via Network/Internet
A tool to monitor normalized and raw values for (critical) [S.M.A.R.T.](
http://en.wikipedia.org/wiki/S.M.A.R.T.) parameters directly/locally (only Linux)
or as [BSD sockets](http://en.wikipedia.org/wiki/Berkeley_sockets) client
(Windows, Linux).
S.M.A.R.T. values are retrieved periodically in an interval.

[comment]: # https://stackoverflow.com/questions/10189356/how-to-add-screenshot-to-readmes-in-github-repository
[comment]: # ![here the screenshot of wxWidgets GUI in Linux Mint should appear](/screenshots/SMARTmonitor_wxGUI_main_dialog.png?raw=true "screenshot of wxWidgets GUI's main dialog in Linux Mint")
![here the screenshot of wxWidgets GUI in Linux Mint should appear](http://it-gebauer.de/SMARTmonitor/SMARTmonitor_wxGUI_main_dialog_Linux_Mint.png "screenshot of wxWidgets GUI's main dialog in Linux Mint")

### S.M.A.R.T. Parameter Value Unit Auto-Detection
Some units (Power-On Time, Total Data Written, ...) for some
[S.M.A.R.T. attributes](
http://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes) are
tried to be determined.

### Multiple Data Carriers
Also the software is capable to monitor more than 1 storage media/data carrier
at the same time.

### Usage Of Appropriate Values
Basically this application uses the normalized current values for detecting how
critical the corresponding S.M.A.R.T. parameter is as rating.
But in some cases, for example the temperature, it is more senseful to rely on
raw values. Because some manufacturers don't follow the convention that lower
normalized current values for a S.M.A.R.T. parameter mean a worse state.

## Debian(-based) Linux

For Debian(-based) Linux 3 packages: common files, client, service can
be created with included shell scripts.

Both GUI/client and daemon/server/service depend on the "common files" (data
files, configuration files) package.

## systemd
For systemd the service can be installed (for example using Debian package
post-installation script).

## Building Prerequisites

general:

* [CMake](https://cmake.org/download) for creating makefile / IDE project files
* C++ compiler (source code is able to build with "g++" 7.5.0)
* "[common_sourcecode](https://www.github.com/st-gb/common_sourcecode)" git
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
    apt-get install libwxbase3.0-dev
    apt-get install libwxgtk3.0-dev
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

Copyright by Stefan Gebauer, no commercial use without grant.
If you want to use the code in a commercial product then contact the author.

The source code is also provided to enable customization of the software.

You may use the code free of charge

- to study the implementation and quality (of one of the authour's software
  products) so bugs can be reported.
- to build targets (wxWidgets GUI, Linux service [...] ) of the software in
  order to test it, especially if a binary is not provided for another
  platform (CPU, Operating System)
- in a non-commercial product if it is not in competition with an author's
  software product (repositories from http://github.com/st-gb).
  
  If in doubt which applications this affects then ask the author.

It is appreciated to reference the author (e.g. in the "about" dialog or
README.txt) if using this code. Therefore please refer to this github user page
and/or to "it-gebauer.de".
