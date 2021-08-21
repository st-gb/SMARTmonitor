message("Calling shell script from CMake: " ${resourcesFSpath} )
set(FullExePath "/usr/${exeInstallDir}/${EXE_NAME}")

set(cmdLine "../Linux/systemd/create_.service_file_with_default_parameters_script.sh \
${resourcesFSpath} \
${FullExePath}")
message("command line to exec:" ${cmdLine})
#https://stackoverflow.com/questions/35072473/executing-bash-commands-from-a-cmake-file/42712713
execute_process(
  COMMAND bash -c ${cmdLine}
  OUTPUT_VARIABLE outVar
  )

set(cmdLine "${localResourcesFSpath}/Linux/systemd/create_.service_file_default_parameters.sh")
message("command line to exec:" ${cmdLine})
#With "bash -c" the "RESULT_VARIABLE" variable is an error code
#Without "bash -c" the "RESULT_VARIABLE" variable is an error character string.
execute_process(
  COMMAND #bash -c
  ${cmdLine}
  RESULT_VARIABLE rsltVar
  )

#${SGR_Green},${SGR_Red},${SGR_ColourReset}
include(${cmnSrc}/CMake/SGR_terminalColours.cmake)

#For the same error in Arch Linux armv7l:
#-was "Permission denied" without "bash -c" in "execute_process"
#-was error code/errno "126"? with "bash -c" in "execute_process"
if(# ${rsltVar} EQUAL 0 )
  "${rsltVar}" STREQUAL "")
  message("${SGR_Green}result of calling \"${cmdLine}\": ${rsltVar}${SGR_ColourReset}")
else()
  message("${SGR_Red}result of calling \"${cmdLine}\": ${rsltVar}${SGR_ColourReset}")
endif()

