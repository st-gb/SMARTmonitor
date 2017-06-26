#from https://cmake.org/cmake-tutorial/

cmake_minimum_required (VERSION 2.6)
project (SMARTmonitor)

if( NOT DEFINED COMMON_SOURCECODE_ROOT_PATH )
  #By default "common_sourcecode" lies/lays 1 level above VTrans3/src
  set(COMMON_SOURCECODE_ROOT_PATH ${PROJECT_SOURCE_DIR}/../../common_sourcecode)
endif( NOT DEFINED COMMON_SOURCECODE_ROOT_PATH )

#To create executable outside of "src" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/..)

set(CMAKE_LIST_FILE_PATHS
  ${CMAKE_CURRENT_LIST_FILE}
)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_DEBUG_POSTFIX "_debug")
#set(<variable> <value>  ... <valueN>)
#set(COMMON_SOURCECODE_ROOT_PATH /home/sg/SourceCodeManagement/common_sourcecode)
#set(WXWIDGETS_SOURCECODE_ROOT_PATH )
#set(TINYXML2_ROOT_PATH /home/sg/SourceCodeManagement/tinyxml2-master )

#from http://stackoverflow.com/questions/12896988/passing-the-argument-to-cmake-via-command-prompt
# on command line: "-DEXE_TYPE:STRING=wxGUI"
SET(EXE_TYPE "GUI" CACHE STRING "Some user-specified option")
message("executable type to build:${EXE_TYPE}")

#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(WXWIDGETS ON)
  set(SMARTmonClient ON)
endif( ${EXE_TYPE} STREQUAL "wxGUI" )

if( DEFINED WXWIDGETS )
  message("WXWIDGETS is defined")
  #see https://cmake.org/cmake/help/v3.0/command/add_subdirectory.html :
  #additionally use CMakeLists.txt from "wxWidgets" folder
  add_subdirectory(wxWidgets 
    #EXCLUDE_FROM_ALL
  )
endif ( DEFINED WXWIDGETS )

set( INCLUDE_DIRS 
  ${INCLUDE_DIRS} #inlcude the wxWidgets dirs from above
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption
  ${PROJECT_SOURCE_DIR}
  ${TINYXML2_ROOT_PATH}
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
message("TINYXML2_ROOT_PATH: ${TINYXML2_ROOT_PATH}")

set( CXX_DEFINITIONS
  -DCOMPILE_LOGGER_MULTITHREAD_SAFE
  #-D__WXDEBUG__
  -DUSE_OWN_LOGGER
  -DCOMPILE_WITH_LOG
  -DCOMPILE_AS_EXECUTABLE
  -DUNICODE
)

#add_subdirectory(./libConfig
#  #EXCLUDE_FROM_ALL
#  )
message (STATUS "${CMAKE_CURRENT_LIST_FILE} LIBRARIES: ${LIBRARIES}")

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
    ${PROJECT_SOURCE_DIR}/UserInterface/UserInterface.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/POSIX/GetBlockDeviceFiles.c
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/POSIX/multithread/pthreadBasedI_Thread.cpp
  	${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.cpp
	  ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.cpp
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

#Source files needed for both client/GUI and server builds.
set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${LOGGER_SOURCE_FILES}
  ${PROJECT_SOURCE_DIR}/tinyxml2/ConfigLoader.cpp
  ${TINYXML2_ROOT_PATH}/tinyxml2.cpp
  ${PROJECT_SOURCE_DIR}/SMARTmonitorBase.cpp
  ${PROJECT_SOURCE_DIR}/SMARTaccessBase.cpp
  ${PROJECT_SOURCE_DIR}/SMARTvalueFormatter.cpp
  #${PROJECT_SOURCE_DIR}/SMARTvalueProcessorBase.cpp
  #${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/stdtstr.cpp
  #${COMMON_SOURCECODE_ROOT_PATH}/data_structures/Trie/byteTrie/ByteTrie.cpp
  #${COMMON_SOURCECODE_ROOT_PATH}/data_structures/Trie/byteTrie/Trie.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/multithread/I_Thread.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/stdtstr.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/FileSystem/PathSeperatorChar.cpp
)

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/daemon/daemon.cpp
    ${PROJECT_SOURCE_DIR}/service/SMARTmonitorService.cpp
    ${PROJECT_SOURCE_DIR}/service/service_main.cpp
  )
endif(${EXE_TYPE} STREQUAL "UNIX_service")

if( ${EXE_TYPE} STREQUAL "ncursesClient" )
#  add_subdirectory(./ncurses
#    #EXCLUDE_FROM_ALL
#    )  
#    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/ncurses/form/Form.cpp
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${PROJECT_SOURCE_DIR}/curses/NcursesUI.cpp
    ${PROJECT_SOURCE_DIR}/curses/ConnectToServerDialog.cpp
    ${PROJECT_SOURCE_DIR}/curses/MainWindow.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses/color.c
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses/EventQueue.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses/GetChar.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses/MessageBox.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses/textfield.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/ncurses/menu/Menu.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses/InputProcessorStack.cpp
    ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses/Window.cpp
  )
  set( LIBRARIES
    ${LIBRARIES}
    ncurses menu form)
  set(SMARTmonClient ON)
endif(${EXE_TYPE} STREQUAL "ncursesClient" )

if( DEFINED SMARTmonClient )
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${PROJECT_SOURCE_DIR}/socket/SocketOperations.cpp
    ${PROJECT_SOURCE_DIR}/tinyxml2/ProcessSMARTdata.cpp
    ${PROJECT_SOURCE_DIR}/client/SMARTmonitorClient.cpp )
endif( DEFINED SMARTmonClient )

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
  message (STATUS "wxWidgets type: ${wxWidgets_CONFIGURATION}")
endif (DEFINED WXWIDGETS)

if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(EXE_NAME wx${PROJECT_NAME}_GUI)
  set(SOURCE_FILE_PATHS 
    ${SOURCE_FILE_PATHS}
    #${TINYXML2_ROOT_PATH}/tinyxml2.cpp
    )
  add_executable(${EXE_NAME} ${SOURCE_FILE_PATHS})
  #set( LIBRARIES ${LIBRARIES} tinyxml2 )
endif( ${EXE_TYPE} STREQUAL "wxGUI" )

if( ${EXE_TYPE} STREQUAL "ncursesClient")
  set(EXE_NAME ${PROJECT_NAME}_client_ncurses)
  add_executable(${EXE_NAME} ${SOURCE_FILE_PATHS})
endif( ${EXE_TYPE} STREQUAL "ncursesClient" )

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(EXE_NAME ${PROJECT_NAME}_service)
  add_executable(${EXE_NAME} ${SOURCE_FILE_PATHS})
endif( ${EXE_TYPE} STREQUAL "UNIX_service" )

if(DEFINED WXWIDGETS)
  #https://wiki.wxwidgets.org/CMake
  target_link_libraries(${EXE_NAME} ${wxWidgets_LIBRARIES})
endif (DEFINED WXWIDGETS)

message ("EXE_NAME_: ${EXE_NAME} LIBRARIES: ${LIBRARIES}")

target_link_libraries(${EXE_NAME} ${LIBRARIES})

set(ARCHIVE_FILE_PATHS
  ${CMAKE_LIST_FILE_PATHS}
  ${SOURCE_FILE_PATHS}
)

#following: package source files into archive file
set(RELATIVE_SOURCE_FILE_PATH, "")
execute_process(COMMAND rm ${EXE_NAME}_source.zip)
#https://cmake.org/cmake/help/v3.0/command/foreach.html
file(WRITE ${EXE_TYPE}source_file_paths.txt "" )
foreach( SOURCE_FILE_PATH ${ARCHIVE_FILE_PATHS} )
  #see https://cmake.org/cmake/help/v3.0/command/file.html
  file(APPEND ${EXE_TYPE}source_file_paths.txt ${SOURCE_FILE_PATH} )
  file(APPEND ${EXE_TYPE}source_file_paths.txt "\n" )
  #${CMAKE_COMMAND} -E tar "cfv" "sources.zip" --format=zip ${SOURCE_FILE_PATH}
  execute_process(COMMAND cd /home/sg/SourceCodeManagement/ )
  #see https://cmake.org/pipermail/cmake/2004-December/005934.html
  string(REGEX REPLACE "^/home/sg/SourceCodeManagement/" "" 
    RELATIVE_SOURCE_FILE_PATH ${SOURCE_FILE_PATH} )
  #message (STATUS "RELATIVE_SOURCE_FILE_PATH:" ${RELATIVE_SOURCE_FILE_PATH} )
  #execute_process(
    #COMMAND cd /home/sg/SourceCodeManagement/ 
    #COMMAND zip ${EXE_NAME}source.zip ./${RELATIVE_SOURCE_FILE_PATH} 
  #  COMMAND tar --strip 3 --append -f ${EXE_NAME}_source.zip ${SOURCE_FILE_PATH} 
  #)
endforeach()

#http://stackoverflow.com/questions/7050997/zip-files-using-cmake
add_custom_target(create_zip COMMAND
    ${CMAKE_COMMAND} -E tar "cfv" "sources.zip" --format=zip
       ${SOURCE_FILE_PATHS}
  )

#cpack --config CPackSourceConfig.cmake
#http://stackoverflow.com/questions/17495906/copying-files-and-including-them-in-a-cpack-archive
#https://cmake.org/Wiki/CMake/CPackExample
#INSTALL(SOURCE ${SOURCE_FILE_PATHS} DESTINATION source)
set(CPACK_GENERATOR "ZIP")
include(CPack)

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
