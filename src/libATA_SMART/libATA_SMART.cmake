#These definitions should be usable by tests und client/server.
#IDEA: make as function in order to support own variable names:
#function(libATA_SMARTAccssSrcFiles libATA_SMARTAccssLibs)
set(CmnSrcOS ${cmnSrcDir}/OperatingSystem)

set(directSMARTaccessSrcFilePaths
  ${directSMARTaccessSrcFilePaths}
  ${CMAKE_CURRENT_LIST_DIR}/SMARTaccess.cpp
  #GetBlockDeviceFiles(...) for ReadSMARTValuesForAllDrives(...)
  ${CmnSrcOS}/POSIX/GetBlockDeviceFiles.c
)

set(CMakeScriptFilePaths
  ${CMakeScriptFilePaths}
  ${CMAKE_CURRENT_LIST_FILE}
)

message("Needs package libatasmart*")
set( LIBRARIES
  ${LIBRARIES}
  #Unix-based:needs package "libatasmart*"? Debian-based:"apt-get install"
  atasmart
)
#endfunction()
