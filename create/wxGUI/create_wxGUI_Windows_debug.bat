@echo off

REM https://stackoverflow.com/questions/1291941/batch-files-number-of-command-line-arguments
set argCount=0
for %%x in (%*) do Set /A argCount+=1
echo number of command line args to this script: %argCount%

REM IF %argCount% GTR 1 (
REM "if ... else" with "( )" did not work, so use dirty "goto"
REM LSS means: less/<
IF %argCount% LSS 2 goto tooFewCmdLineArgs

REM ) ELSE (
REM param 1: path to Msys/Cygwin, e.g. C:\MinGW\5.1.6\msys\1.0\bin
REM @echo PATH: %PATH%
echo "Adding 1st command line argument (for "readlink" etc.) : "%1" to "PATH" environment variable"
set origPATH=%PATH%
REM Need to quote assignment if "if else" blocks to avoid block end if ")" in variable?
@PATH=%1;%PATH%
REM e.g. E:/wxWidgets/MSW-3.1.0_gcc510TDM_x64_Dev
set wxRootDir=%2
set CMakeBuildSysGenerator=%3
echo using "%wxRootDir%" as wxWidgets root directory for header files and "findwxWidgets.cmake"
REM @echo ^PATH: %PATH%
bash.exe ../create/wxGUI/create_wxGUI_Windows_debug.sh %wxRootDir% %CMakeBuildSysGenerator%
REM Reset PATH to orginal to prevent getting larger for current terminal if
REM this script is this called multiple times
set PATH=%origPATH%
goto eof
REM ) else (
REM )

:tooFewCmdLineArgs
echo Error: less than 2 command line arguments passed to this script
echo Pass at least: 1. path containing "readlink" 2. wxWidgets root directory

:eof
if errorlevel 1 (
 echo Error executing subscript.
 echo PATH: %PATH%
 )
