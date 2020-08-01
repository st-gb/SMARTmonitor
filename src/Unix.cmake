set(CmnSrcOS ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem)

if(multithread)
  set(LIBRARIES
    ${LIBRARIES}
    pthread#(Only) needed if multithreaded
   )
  set(CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    ${CmnSrcOS}/POSIX/multithread/pthreadBasedI_Thread.cpp
  )
else()
  message(${CMAKE_CURRENT_LIST_FILE}":multithread not defined")
endif()

set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${CmnSrcOS}/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.cpp
  ${CmnSrcOS}/Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.cpp
)
#Needed for direct access to S.M.A.R.T. data, i.e. at least for server/service
if(directSMARTaccess)
  include(${CMAKE_CURRENT_LIST_DIR}/libATA_SMART/libATA_SMART.cmake)
  set(CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    ${directSMARTaccessSrcFilePaths})
else()
  message("directSMARTaccess not defined")
endif()
#message("CLIENT_SERVER_BASE_SOURCE_FILES: ${CLIENT_SERVER_BASE_SOURCE_FILES}")