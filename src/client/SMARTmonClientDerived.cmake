
set(SMARTattrValProcSrcFilePaths 
  ${SMARTmonSrcDir}/SMARTvalueFormatter.cpp)
set(CmnSrcSocket ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/BSD/socket)
#Source files needed for a SMARTmonitorClient(-derived) class
set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${SMARTattrValProcSrcFilePaths}
  ${SMARTmonSrcDir}/socket/SocketOperations.cpp
  ${CmnSrcSocket}/prepCnnctToSrv.c
  ${CmnSrcSocket}/socketTimeout.cpp
  ${SMARTmonSrcDir}/tinyxml2/ProcessSMARTdata.cpp
  ${SMARTmonSrcDir}/client/SMARTmonitorClient.cpp
)
