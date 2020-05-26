#from https://cmake.org/cmake-tutorial/
#This file should be structured so that all subprojects/tests can be build with
#shared/common CMake include (.cmake) files.

#https://stackoverflow.com/questions/15662497/difference-between-cmake-current-source-dir-and-cmake-current-list-dir
# https://cmake.org/cmake/help/v2.8.3/cmake.html#variable:CMAKE_CURRENT_LIST_DIR
# : CMAKE_CURRENT_LIST_DIR used in wxWidgetsGUI.cmake since CMake 2.8.3
cmake_minimum_required(VERSION 2.8.3)
project(SMARTmonitor)

set(SMARTmonSrcDir ${PROJECT_SOURCE_DIR})

#To create executable outside of "src" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/..)

#Add all CMake script file paths to this variable (for including in an archive).
set(CMakeScriptFilePaths
  ${CMAKE_CURRENT_LIST_FILE}#this CMake file
)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_DEBUG_POSTFIX "_debug")
#set(<variable> <value>  ... <valueN>)
#set(WXWIDGETS_SOURCECODE_ROOT_PATH )

#from http://stackoverflow.com/questions/12896988/passing-the-argument-to-cmake-via-command-prompt
# on command line: "-DEXE_TYPE:STRING=wxGUI"
SET(EXE_TYPE "GUI" CACHE STRING "Some user-specified option")
message("executable type to build:${EXE_TYPE}")

#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(WXWIDGETS ON)
  set(SMARTmonClientDerived ON)
endif( ${EXE_TYPE} STREQUAL "wxGUI" )

include(clientSrvSrcFiles.cmake)
if(DEFINED WXWIDGETS)
  message("WXWIDGETS is defined")
  include(wxWidgets/wxWidgetsGUI.cmake)
endif()

#set(PROJECT_BINARY_DIR ../build)
#http://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir
#set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/../build)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")
#set(CMAKE_CURRENT_BINARY_DIRECTORY ../bin)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message("TINYXML2_ROOT_PATH: ${TINYXML2_ROOT_PATH}")

message( "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )
if( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set( CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -D_DEBUG
    )
endif()
message( "CXX_DEFINITIONS: ${CXX_DEFINITIONS}" )

#add_subdirectory(./libConfig
#  #EXCLUDE_FROM_ALL
#  )
message (STATUS "${CMAKE_CURRENT_LIST_FILE} LIBRARIES: ${LIBRARIES}")

#include_directories("${PROJECT_BINARY_DIR}")

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

set( INCLUDE_DIRS 
  ${INCLUDE_DIRS} #include the wxWidgets dirs from above
  ${InclDirs}
)
message("include directories: ${INCLUDE_DIRS}")

#https://www.johnlamp.net/cmake-tutorial-4-libraries-and-subdirectories.html
include_directories( ${InclDirs} )


if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/daemon/daemon.cpp
    ${PROJECT_SOURCE_DIR}/service/SMARTmonitorService.cpp
    ${PROJECT_SOURCE_DIR}/service/service_main.cpp
  )
endif(${EXE_TYPE} STREQUAL "UNIX_service")

if( ${EXE_TYPE} STREQUAL "ncursesClient" )
  include(curses/cursesClient.cmake)
endif(${EXE_TYPE} STREQUAL "ncursesClient" )

set(TinyXML2dir ../../tinyxml2-master)

if(DEFINED SMARTmonClientDerived)#define for all clients
  include(client/SMARTmonClientDerived.cmake)
endif()

add_definitions(${CXX_DEFINITIONS})
#add_library(${LIBRARIES} )
#specify which libraries an executable target links against.
#TARGET_LINK_LIBRARIES( ${LIBRARIES})

#target_link_libraries(${LIBRARIES})

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

#TODO source out to a separate file:
#include(${CmnSrc}/cr8src_archive.cmake)
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
