@echo off
REM https://stackoverflow.com/questions/1291941/batch-files-number-of-command-line-arguments
set argCount=0
for %%x in (%*) do Set /A argCount+=1
echo number of command line args to this script: %argCount%

set numConcurrentCompiles=1
IF %argCount% LSS 1 (
  echo Pass the number of concurrent compile jobs to this script as 1st command line argument
  echo Setting default value 1 for "number of concurrent compile jobs"
) else set /a numConcurrentCompiles=%1

echo number of concurrent compile processes(1st cmd line arg):%numConcurrentCompiles%

mingw32-make -f Makefile -j%numConcurrentCompiles%
echo errorlevel: %errorlevel%
REM https://ss64.com/nt/errorlevel.html: if at least return code 1
if errorlevel 1 (
 echo Error executing "mingw32-make". Is it in PATH environment variable?
 echo PATH: %PATH%
 )
