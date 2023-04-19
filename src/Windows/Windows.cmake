set(CmnSrcWinOS ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Windows)

set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${CmnSrcWinOS}/Logger/LogEntryOutputter.cpp
  ${CmnSrcWinOS}/ErrorCode/GetErrorMessageFromErrorCode.cpp
  ${CmnSrcWinOS}/ErrorCode/LocalLanguageMessageFromErrorCode.cpp
  ${CmnSrcWinOS}/multithread/Thread.cpp
)

##The following definitions are needed
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
## http://wiki.wxwidgets.org/Microsoft_Visual_C%2B%2B_Guide#Project_properties :
  add_definitions("-D__WXDEBUG__")
endif()
if(${EXE_TYPE} STREQUAL "wxGUI")
  ##Without when building wxGUI: error: unresolved symbol "main" in function
  ## ""int __cdecl invoke_main(void)" "". (->console application<=>"main"
  ## function is assumed). "/SUBSYSTEM:WINDOWS" uses "WinMain".
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()
## http://wiki.wxwidgets.org/Microsoft_Visual_C%2B%2B_Guide#Project_properties :
add_definitions("-DWXUSINGDLL")
