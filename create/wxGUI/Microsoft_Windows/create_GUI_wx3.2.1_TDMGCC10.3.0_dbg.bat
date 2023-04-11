@echo off
REM Customize this file for GCC compiler under Microsoft Windows to fit your
REM  file systems directories/paths and library names.
cmake -G "Unix Makefiles" ^
-DCOMMON_SOURCECODE_ROOT_PATH=D:\SrcCdMgmt\common_sourcecode ^
-DTINYXML2_ROOT_PATH=D:/Libs/tinyxml2/tinyxml2-master ^
-DCMAKE_C_COMPILER=D:/TDM-GCC/64/10.3.0-2/bin/gcc.exe ^
-DCMAKE_CXX_COMPILER="D:/TDM-GCC/64/10.3.0-2/bin/g++.exe" ^
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
-DEXE_TYPE:STRING=wxGUI ^
-DWXWIDGETS_INCLUDE_DIR:STRING=D:\wxWidgets\3.2.1\include ^
-DWXWIDGETS_SETUP_H_PATH:STRING=D:\wxWidgets\3.2.1\GCC10.3.0TDM_x64\lib\gcc_dll\mswud ^
-DwxWidgets_ROOT_DIR=D:\wxWidgets\3.2.1\GCC10.3.0TDM_x64 ^
-DwxLibDir=D:\wxWidgets\3.2.1\GCC10.3.0TDM_x64\lib\gcc_dll ^
-DwxLibs="libwxbase32ud.a;libwxmsw32ud_adv.a;libwxmsw32ud_core.a" ^
-Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug -DSetwxPathsManually=TRUE ^
.

REM -DTU_Bln361095useBSDskt=OFF ^
