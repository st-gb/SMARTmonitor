#from https://cmake.org/cmake-tutorial/

cmake_minimum_required (VERSION 2.6)
project (wxSMARTmonitor)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_DEBUG_POSTFIX "_debug")
#set(<variable> <value>  ... <valueN>)
set(COMMON_SOURCECODE_ROOT_PATH /home/sg/SourceCodeManagement/common_sourcecode)
set(WXWIDGETS_SOURCECODE_ROOT_PATH )
set(VTRANS3_ROOT_PATH ${PROJECT_SOURCE_DIR} )

#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
#Set wxWidgets_USE_DEBUG to "ON" to link against WxWidgets debug libraries.
set( wxWidgets_USE_DEBUG ON)

#Set to appropriate version--releae, debug, 32/64 bit
set( WXWIDGETS_SETUP_H_HEADER_PATH /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-2.8)
set( WXWIDGETS_HEADER_PATH /usr/include/wx-2.8)

set( INCLUDE_DIRS 
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption
  ${WXWIDGETS__SETUP_H_HEADER_PATH}
  ${PROJECT_SOURCE_DIR}
  ${WXWIDGETS_HEADER_PATH}
)
message("include directories: ${INCLUDE_DIRS}")

#set(PROJECT_BINARY_DIR /home/sg/SourceCodeManagement/VTrans3/build)
#http://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir
#set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/../build)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")
#set(CMAKE_CURRENT_BINARY_DIRECTORY /home/sg/SourceCodeManagement/VTrans3/bin)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message("wxWidgets_USE_DEBUG: ${wxWidgets_USE_DEBUG}")

set( CXX_DEFINITIONS
  -DCOMPILE_LOGGER_MULTITHREAD_SAFE
  #-D__WXDEBUG__
  -DUSE_OWN_LOGGER
  -DCOMPILE_WITH_LOG
  -DCOMPILE_AS_EXECUTABLE
  -DUNICODE
)

set(WXWIDGETS ON)
if( DEFINED WXWIDGETS )
  #https://cmake.org/cmake/help/v3.0/command/add_subdirectory.html
  add_subdirectory(wxWidgets 
    #EXCLUDE_FROM_ALL
  )
endif ( DEFINED WXWIDGETS )
add_subdirectory(./libConfig
  #EXCLUDE_FROM_ALL
  )

#include_directories("${PROJECT_BINARY_DIR}")

#https://www.johnlamp.net/cmake-tutorial-4-libraries-and-subdirectories.html
include_directories( ${INCLUDE_DIRS} )
#if( DEBUG)
#  /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-2.8

#"Add directories to the end of this directory’s include paths."
#https://cmake.org/Wiki/CMake_Useful_Variables for "ENV
#include_directories($ENV{COMMON_SOURCECODE_DIR})

#https://cmake.org/cmake/help/v3.0/command/add_executable.html
#add_executable(VTrans ${PROJECT_SOURCE_DIR})

#http://stackoverflow.com/questions/17653738/recursive-cmake-search-for-header-and-source-files
#file(GLOB_RECURSE SOURCE_FILE_PATHS *.cpp)

#http://stackoverflow.com/questions/16449676/remove-specific-file-from-cmake-build
#list(REMOVE_ITEM SOURCE_FILE_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
#LIST(FILTER <list> <regex> [<regex> ...] [OUTPUT_VARIABLE <variable>])
#set(SOURCE_FILE_PATHS_TEMP ${SOURCE_FILE_PATHS})
#list(REMOVE_ITEM SOURCE_FILE_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/testing.cpp)

set( ATTRIBUTE_DATA_SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/attributes/SMARTattributeNameAndID.cpp
  ${PROJECT_SOURCE_DIR}/attributes/SMARTentry.cpp
  ${PROJECT_SOURCE_DIR}/attributes/SMARTuniqueID.cpp
  ${PROJECT_SOURCE_DIR}/attributes/SMARTuniqueIDandValues.cpp
)

if (UNIX)
  message("UNIX defined")
  set( LIBRARIES
    ${LIBRARIES}
    pthread
  )
  add_subdirectory(./libATA_SMART
    #EXCLUDE_FROM_ALL
    )
  set( CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    ${COMMON_SOURCECODE_ROOT_PATH}/Linux/multithread/pthreadBasedI_Thread.cpp
  	${COMMON_SOURCECODE_ROOT_PATH}/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.cpp
	  ${COMMON_SOURCECODE_ROOT_PATH}/Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.cpp
  )
  #message("CLIENT_SERVER_BASE_SOURCE_FILES: ${CLIENT_SERVER_BASE_SOURCE_FILES}")
endif (UNIX)

set(LOGGER_SOURCE_FILES
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/preprocessor_logging_macros.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Logger.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/LogLevel.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Appender/FormattedLogEntryProcessor.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Appender/RollingFileOutput.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/I_LogFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/HTMLlogFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/Log4jFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/I_LogEntryOutputter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/StdCoutLogWriter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/StdOfStreamLogWriter.cpp
)

set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${LOGGER_SOURCE_FILES}
  ${PROJECT_SOURCE_DIR}/SMARTaccessBase.cpp
  ${PROJECT_SOURCE_DIR}/SMARTvalueProcessorBase.cpp
  #${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/stdtstr.cpp
  #${COMMON_SOURCECODE_ROOT_PATH}/data_structures/Trie/byteTrie/ByteTrie.cpp
  #${COMMON_SOURCECODE_ROOT_PATH}/data_structures/Trie/byteTrie/Trie.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/multithread/I_Thread.cpp
)

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${CONFIGURATION_READER_SOURCE_FILES}
  ${ATTRIBUTE_DATA_SOURCE_FILES}
)

add_definitions(${CXX_DEFINITIONS})
#add_library(${LIBRARIES} )
#specify which libraries an executable target links against.
#TARGET_LINK_LIBRARIES( ${LIBRARIES})

#target_link_libraries(${LIBRARIES})

if(DEFINED WXWIDGETS)
  #https://wiki.wxwidgets.org/CMake
  set(wxWidgets_CONFIGURATION mswud)
  SET(wxWidgets_USE_LIBS core base adv)
  find_package(wxWidgets COMPONENTS core base adv REQUIRED)
  include(${wxWidgets_USE_FILE})

  message("wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")
endif (DEFINED WXWIDGETS)

add_executable(wxSMARTmonitor ${SOURCE_FILE_PATHS})

if(DEFINED WXWIDGETS)
  #https://wiki.wxwidgets.org/CMake
  target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif (DEFINED WXWIDGETS)

message ("LIBRARIES: ${LIBRARIES}")

target_link_libraries(wxSMARTmonitor ${LIBRARIES})
message (STATUS "wxWidgets type: ${wxWidgets_CONFIGURATION}")


#target_link_libraries (VTrans3 wxWidgets)

#http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

#https://cmake.org/pipermail/cmake/2006-October/011548.html
set(CPACK_SOURCE_IGNORE_FILES
"Makefile\\\\.in$"
"~$"
"^${PROJECT_SOURCE_DIR}.*/CVS/"
"^${PROJECT_SOURCE_DIR}/debian/"
"^${PROJECT_SOURCE_DIR}/old/"
)
