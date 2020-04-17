set(CmnSrcOS ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem)

if(multithreaded)
  set( LIBRARIES
    ${LIBRARIES}
    pthread#(Only) needed if multithreaded
   )
  set(CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    ${CmnSrcOS}/POSIX/multithread/pthreadBasedI_Thread.cpp
  )
else()
  message("multithreaded not defined")
endif()

set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${CmnSrcOS}/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.cpp
  ${CmnSrcOS}/Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.cpp
)
#Needed for direct access to S.M.A.R.T. data, i.e. at least for server/service
if(directSMARTaccess)
  add_subdirectory(./libATA_SMART
    #EXCLUDE_FROM_ALL
    )
  set( CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    #GetBlockDeviceFiles(...) for ReadSMARTValuesForAllDrives(...)
    ${CmnSrcOS}/POSIX/GetBlockDeviceFiles.c
  )
else()
  message("directSMARTaccess not defined")
endif()
#message("CLIENT_SERVER_BASE_SOURCE_FILES: ${CLIENT_SERVER_BASE_SOURCE_FILES}")