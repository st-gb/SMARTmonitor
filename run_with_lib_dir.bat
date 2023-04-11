@rem Disable output of commands to standard output/console.
@echo off

set cmnSrcDir=..\common_sourcecode
cmd /C %cmnSrcDir%\OperatingSystem\DOS\batch_files\getNumCmdLneArgs.bat

echo Number of command line arguments: %cmdLneArgCnt%

rem for example ".\build\wxSMARTmonitor_GUI_Debug.exe"
set exePath=%1
rem for example "D:\wxWidgets\3.2.1\GCC10.3.0TDM_x64\lib\gcc_dll"
set libDir=%2

set origPATH=PATH
set PATH=%libDir%;%PATH%

echo %TIME%: starting executable "%exePath%"
%exePath%

rem Restore original "PATH" environment variable.
set PATH=%origPATH%