
# https://medium.com/@mononz/custom-launcher-for-run-installs-on-ubuntu-debian-660bd09218bd

if(UNIX)
  message("Calling shell script from CMake: " ${EXE_NAME})
  #https://stackoverflow.com/questions/35072473/executing-bash-commands-from-a-cmake-file/42712713
  execute_process(
    COMMAND bash -c "../Linux/createOSmenuItemFile.sh /usr/${exeInstallDir}/${EXE_NAME} ${resourcesFSpath}/icons/SMARTmonitor.desktop"
    OUTPUT_VARIABLE outVar
    )
endif()

INSTALL(FILES ${localResourcesFSpath}/Linux/SMARTmonitor.desktop
  #required
  DESTINATION /usr/share/applications )