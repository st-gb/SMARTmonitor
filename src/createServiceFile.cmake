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
execute_process(
  COMMAND bash -c ${cmdLine}
  RESULT_VARIABLE rsltVar
  )
message("result of calling \"${cmdLine}\": ${rsltVar}")
