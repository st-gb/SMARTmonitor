
set(SMARTattrValProcSrcFilePaths 
  ${SMARTmonSrcDir}/SMARTvalueFormatter.cpp)
#Source files needed for a SMARTmonitorClient(-derived) class
set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${SMARTattrValProcSrcFilePaths}
  ${SMARTmonSrcDir}/client/SMARTmonitorClient.cpp
  #${TU_Bln361095cmnSrcRootDir}/hardware/dataCarrier/NVMe/enSMARTattrNames.c
  ${TU_Bln361095cmnSrcRootDir}/hardware/dataCarrier/NVMe/enSMARTattrNames.cpp
  #${SMARTmonSrcDir}/UserInterface/SMARTparamTblHdrWStr.c
  ${SMARTmonSrcDir}/UserInterface/SMARTattrTblHdrWStr.cpp
)

#if(DEFINED TU_Bln361095useBSDskt AND TU_Bln361095useBSDskt EQUAL TRUE)
if(TU_Bln361095useBSDskt)##variable defined and not false
 message("${SGR_Blue}UI should use BSD sockets${SGR_ColourReset}")
 set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${SMARTmonSrcDir}/client/InterProcComm/BSDskt/SMARTmonUI_BSDskt.cpp
  )
  ##BSD sockets are a subtype of Client/Server and InterProcess Communication
  set(TU_Bln361095useClntSrv TRUE)
  set(compDefs "-DTU_Bln361095useBSDskt -DTU_Bln361095useClntSrv \
    -DTU_Bln361095useInterProcComm")
  add_definitions(${compDefs})
  message("adding comp. defs:${compDefs}")
  include(${TU_Bln361095cmnSrcRootDir}/OperatingSystem/BSD/socket/set_IPv.cmake)
endif()
if(DEFINED TU_Bln361095useClntSrv)
 set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${SMARTmonSrcDir}/tinyxml2/ProcessSMARTdata.cpp
  )
endif()
