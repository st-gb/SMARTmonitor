#Source files needed for a SMARTmonitorClient(-derived) class
set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${PROJECT_SOURCE_DIR}/socket/SocketOperations.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/socket/prepCnnctToSrv.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/socket/socketTimeout.cpp
  ${PROJECT_SOURCE_DIR}/tinyxml2/ProcessSMARTdata.cpp
  ${PROJECT_SOURCE_DIR}/client/SMARTmonitorClient.cpp
)