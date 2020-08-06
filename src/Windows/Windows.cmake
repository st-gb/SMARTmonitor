set(CmnSrcWinOS ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Windows)

set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${CmnSrcWinOS}/Logger/LogEntryOutputter.cpp
  ${CmnSrcWinOS}/ErrorCode/GetErrorMessageFromErrorCode.cpp
  ${CmnSrcWinOS}/ErrorCode/LocalLanguageMessageFromErrorCode.cpp
  ${CmnSrcWinOS}/multithread/Thread.cpp
)