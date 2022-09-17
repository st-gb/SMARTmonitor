
set(SMARTattrValProcSrcFilePaths 
  ${SMARTmonSrcDir}/SMARTvalueFormatter.cpp)
#Source files needed for a SMARTmonitorClient(-derived) class
set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${SMARTattrValProcSrcFilePaths}
  ${SMARTmonSrcDir}/client/SMARTmonitorClient.cpp
)

if(DEFINED TU_Bln361095useBSDskt)
 set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${SMARTmonSrcDir}/socket/SocketOperations.cpp
  )
endif()
if(DEFINED TU_Bln361095useClntSrv)
 set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${SMARTmonSrcDir}/client/clntSrv.cpp
  )
endif()