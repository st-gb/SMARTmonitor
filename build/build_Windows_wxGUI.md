# Buildung wxWidgets GUI of S. Gebauer's SMARTmonitor application under Microsoft Windows

## General Prerequisites

See [General Build instructions](../BUILD.md)

Download the wxWidgets header files/source code and libraries for Microsoft Windows [here](http://www.wxwidgets.org/downloads) (tested with version 3)

## 1 wxWidgets Configuration

Building the software with Microsoft Windows is more complicated than with Linux
because wxWidgets is not installed via a package manager with standard/default
file paths as under Linux but instead usually downloaded as compressed folder
and extracted.

### 1.1 Automatic Configuration with [CMake](https://en.wikipedia.org/wiki/CMake)

CMake can use the script "FindwxWidgets.cmake" (may be in
\>>CMake root directory<<\share/cmake->>version<</Modules/ directory) to get the
wxWidgets library file names for linking.

But using CMake for wxWidgets is more error-prone because paths don't fit
the naming scheme of CMake's "FindwxWidgets.cmake".

So error messages may be:

- " Could NOT find wxWidgets (missing: wxWidgets\_LIBRARIES
  wxWidgets\_INCLUDE\_DIRS)"  
  Because the path may be something like
  \>>wxWidgets\_ROOT\_DIR<</lib/**gcc1030TDM\_x64\_dll**/mswu
  rather than the expected path
  \>>wxWidgets\_ROOT\_DIR<</lib/**gcc_dll**/mswu
- "wxWidgets wx/version.h file not found in 
  D:/wxWidgets/3.2.1/GCC10.3.0TDM\_x64/lib/gcc\_dll/mswu."
  
### 1.2 Manual Configuration

Pass "-DwxLibs=_libraries separated by ";"_" to CMake

Unicode Release libraries: "u" in file name, for example "libwxbase32u.a"
Unicode Debug libraries: "ud" in file name, for example "libwxbase32ud.a"

For building **_U_nicode _d_ebug** version, pass e. g. to CMake:
"-DwxLibs=libwxbase32**ud**.a;libwxmsw32**ud**\_adv.a;libwxmsw32**ud**\_core.a"

#### 1.2.1 Sample Build:
Command in "[cmd](https://en.wikipedia.org/wiki/Cmd.exe)" shell window (there as
1 line without line endings, here with newlines for better
readability):

<!-- https://www.markdownguide.org/basic-syntax/#code-blocks-1 :
  section ""When they’re in a list, indent them eight spaces or two tabs." -->

1. Generate Makefile

    Execute CMake in \>>SMARTmonitor root directory<</src directory
    within the CMD shell.

    It is recommended to put the command line into a (batch)[
    https://en.wikipedia.org/wiki/Batch_file] file.
    So you can generate the Makefile in a later Microsoft Windows/CMD session.  
    Recommended: choose a file name with
    - wxWidgets:version, character set (Unicode/ANSI, ...), build type
      (debug, release, ...)
    - compiler:type, version

    like "createGUIwx3.1.2unicodeDbgTDM_GCC10.3.0.bat".

    This is a functioning example command line (when file system paths are existing and set accordingly):

        cmake -G "Unix Makefiles"
         -DCOMMON_SOURCECODE_ROOT_PATH=D:\SrcCdMgmt\common_sourcecode  
         -DTINYXML2_ROOT_PATH=D:/Libs/tinyxml2/tinyxml2-master
         -DCMAKE_C_COMPILER=D:/TDM-GCC/64/10.3.0-2/bin/gcc.exe
         -DCMAKE_CXX_COMPILER="D:/TDM-GCC/64/10.3.0-2/bin/g++.exe"
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DEXE_TYPE:STRING=wxGUI
         -DWXWIDGETS_INCLUDE_DIR:STRING=D:\wxWidgets\3.2.1\include
         -DWXWIDGETS_SETUP_H_PATH:STRING=D:\wxWidgets\3.2.1\GCC10.3.0TDM_x64\lib\gcc_dll\mswud
         -DwxWidgets_ROOT_DIR=D:\wxWidgets\3.2.1\GCC10.3.0TDM_x64
         -Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug -DSetwxPathsManually=TRUE  
         -DwxLibs="libwxbase32ud.a;libwxmsw32ud_adv.a;libwxmsw32ud_core.a"
         -D_UNICODE
         .
2. execute "make": see "Makefiles" in [General Build instructions](../BUILD.md#makefile)