@REM Disable output ("echoing") of commands.
@echo off
echo You must run this script from within the file system directory that
echo contains the "CMakeLists.txt" file.
echo Therefore change to this file system directory and from there prepend the
echo file system directory of THIS file like:
echo "..\create\wxGUI\Microsoft_Windows\createDbgGUIwx3.2.1VisualStudio2022.bat"

set additionalCMakeArgs=%1
echo additionalCMakeArgs: %additionalCMakeArgs%

REM Customize this file for Microsoft Visual Studio (under Microsoft Windows)
REM  to fit your file systems directories/paths and library names.
REM Call this file from "src" subdirectory.
cmake -G "Visual Studio 17 2022" ^
-DTU_Bln361095cmnSrcRootDir=D:\SrcCdMgmt\common_sourcecode ^
-DTINYXML2_ROOT_PATH=D:/Libs/tinyxml2/tinyxml2-master ^
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
-DEXE_TYPE:STRING=wxGUI ^
-DWXWIDGETS_INCLUDE_DIR:STRING=D:\wxWidgets\3.2.1\include ^
-DWXWIDGETS_SETUP_H_PATH:STRING=D:\wxWidgets\3.2.2\VC14xReleaseDLLx64\lib\vc14x_x64_dll\mswud ^
-DwxWidgets_ROOT_DIR=D:\wxWidgets\3.2.2\VC14xReleaseDLLx64 ^
-DwxLibDir=D:\wxWidgets\3.2.2\VC14xReleaseDLLx64\lib\vc14x_x64_dll ^
-DwxLibs="wxbase32ud.lib;wxmsw32ud_adv.lib;wxmsw32ud_core.lib" ^
-DTU_Bln361095SMARTmonMultithread=ON ^
-DCMAKE_BUILD_TYPE=Debug ^
-DSetwxPathsManually=TRUE ^
-DTU_Bln361095SMARTmonDrctSMARTaccss=TRUE ^
-DBoostRootDir=D:\Boost1.81.0\boost_1_81_0 ^
.

REM -DTU_Bln361095useBSDskt=OFF ^
