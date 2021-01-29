
# https://medium.com/@mononz/custom-launcher-for-run-installs-on-ubuntu-debian-660bd09218bd

if(UNIX)
  message("Calling shell script from CMake: " ${EXE_NAME})
  set(cmdLine "../Linux/createOSmenuItemFile.sh \
/usr/${exeInstallDir}/${EXE_NAME} \
${resourcesFSpath}/icons/S.M.A.R.T._OK.xpm")
  message("command line to exec:" ${cmdLine})
  #https://stackoverflow.com/questions/35072473/executing-bash-commands-from-a-cmake-file/42712713
  execute_process(
    COMMAND bash -c ${cmdLine}
    OUTPUT_VARIABLE outVar
    )
endif()

INSTALL(FILES ${localResourcesFSpath}/Linux/SMARTmonitor.desktop
  #required
  DESTINATION /usr/share/applications )