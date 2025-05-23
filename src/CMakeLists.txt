#from https://cmake.org/cmake-tutorial/
#This file should be structured so that all subprojects/tests can be build with
#shared/common CMake include (.cmake) files.

#https://stackoverflow.com/questions/15662497/difference-between-cmake-current-source-dir-and-cmake-current-list-dir
# https://cmake.org/cmake/help/v2.8.3/cmake.html#variable:CMAKE_CURRENT_LIST_DIR
# : CMAKE_CURRENT_LIST_DIR used in wxWidgetsGUI.cmake since CMake 2.8.3
cmake_minimum_required(VERSION 2.8.3)
project(SMARTmonitor)

set(SMARTmonSrcDir ${PROJECT_SOURCE_DIR})

#To create executable outside of "src" directory;also used for creating man-page
# for "cmnfiles" debian package.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../build)

set(cre8CmnFilesDebPkg FALSE)
##Don't treat quoted character string as variable.
cmake_policy(SET CMP0054 NEW)
if(DEFINED EXE_TYPE)
  if(${EXE_TYPE} STREQUAL "cre8CmnFilesDebPkg")
    set(cre8CmnFilesDebPkg TRUE)
  endif()
endif()
if(cre8CmnFilesDebPkg)
  set(cr8DebPkg ON)
  #The variable for "common_source" path is needed in "createDebPkg.cmake"
  include(${SMARTmonSrcDir}/setCmnSrcDir.cmake)
else()

#Add all CMake script file paths to this variable (for including in an archive).
set(CMakeScriptFilePaths
  ${CMAKE_CURRENT_LIST_FILE}#this CMake file
)

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
if(DEFINED SMARTmonClientDerived)#define for all clients
  ##Must be called before wxWidgetsGUI.cmake because it sets
  ## TU_Bln361095useClntSrv
  include(client/SMARTmonClientDerived.cmake)
endif()

##${SGR_Cyan},${SGR_ColourReset}
include(${cmnSrcDir}/CMake/SGR_terminalColours.cmake)

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
message("${SGR_Cyan}CXX_DEFINITIONS${SGR_ColourReset}:\"${SGR_Magenta}"
  "${CXX_DEFINITIONS}${SGR_ColourReset}\"")

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

#https://www.johnlamp.net/cmake-tutorial-4-libraries-and-subdirectories.html
include_directories(${INCLUDE_DIRS})

#message("include directories: ${INCLUDE_DIRS}")
include(${cmnSrcDir}/compiler/C,C++/printInclDirs.cmake)
TU_Bln361095printCmplrInclDirs("CMakeLists.txt")

if(${EXE_TYPE} STREQUAL "UNIX_service")
  include(server/BSD_sockets.cmake)#sets "BSD_sockets_srcFiles"
  #set(TU_Bln361095SMARTmonDrctSMARTaccss TRUE)
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/daemon/daemon.cpp
    ${BSD_sockets_srcFiles}
    ${PROJECT_SOURCE_DIR}/service/service_main.cpp
  )
endif(${EXE_TYPE} STREQUAL "UNIX_service")

if( ${EXE_TYPE} STREQUAL "ncursesClient" )
  include(curses/cursesClient.cmake)
endif(${EXE_TYPE} STREQUAL "ncursesClient" )

set(TinyXML2dir ../../tinyxml2-master)

message("adding preproc macro \"-DCXX_FLAGS=${CMAKE_CXX_FLAGS}\"")
set(CXX_DEFINITIONS "${CXX_DEFINITIONS} -DCXX_FLAGS=\"${CMAKE_CXX_FLAGS}\"")

message("adding preproc macro \"-DCOMPILER=${CMAKE_CXX_COMPILER_ID}\"")
set(CXX_DEFINITIONS "${CXX_DEFINITIONS} -DCXX_COMPILER=${CMAKE_CXX_COMPILER_ID}")

set(CXX_DEFINITIONS "${CXX_DEFINITIONS} -DCXX_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}")
set(CXX_DEFINITIONS "${CXX_DEFINITIONS} -DBUILD_TYPE=${CMAKE_BUILD_TYPE}")

add_definitions(${CXX_DEFINITIONS})
#add_library(${LIBRARIES} )
#specify which libraries an executable target links against.
#TARGET_LINK_LIBRARIES( ${LIBRARIES})

#target_link_libraries(${LIBRARIES})

if(NOT ${EXE_TYPE} STREQUAL "debPkg")
include(createExeName.cmake)
createExeName(${EXE_TYPE})

add_executable(${EXE_NAME} ${SOURCE_FILE_PATHS})

if(DEFINED WXWIDGETS)
  #https://wiki.wxwidgets.org/CMake
  target_link_libraries(${EXE_NAME} ${wxWidgets_LIBRARIES})
endif (DEFINED WXWIDGETS)

message("EXE_NAME: ${EXE_NAME} LIBRARIES: ${LIBRARIES}")

message("CMAKE CXX flags: \"${CMAKE_CXX_FLAGS}\"")
message("CMAKE_CXX_FLAGS_DEBUG: \"${CMAKE_CXX_FLAGS_DEBUG}\"")
message("CFLAGS: ${CFLAGS} CXXFLAGS: ${CXXFLAGS} LDFLAGS: ${LDFLAGS}")
message("CMAKE_EXE_LINKER_FLAGS: \"${CMAKE_EXE_LINKER_FLAGS}\"")

include(${cmnSrcDir}/compiler/C,C++/printLibDirs.cmake)##printLibDirs(...)
printLibDirs(5)

#From https://stackoverflow.com/questions/5403705/cmake-add-definitions-and-compile-definitions-how-to-see-them
#: Output variable set with "add_definitions(...)"
get_directory_property(CompDefs COMPILE_DEFINITIONS )
message("COMPILE_DEFINITIONS (set with \"add_definitions(...)\"): ${CompDefs}")

  target_link_libraries(${EXE_NAME} ${LIBRARIES})
else()
  set(cr8DebPkg TRUE)
endif()

if(cre8SrcArchive)
  include(${CmnSrc}/CMake/cr8srcArchive.cmake)
endif()
endif(cre8CmnFilesDebPkg)#"else" branch of condition
if(UNIX)#only Unix binaries into .deb file
  message("cr8DebPkg: ${cr8DebPkg}")
  if(DEFINED cr8DebPkg)#"-Dcr8DebPkg" as cmake command line parameter
    include(Linux/createDebPkg.cmake)#run CPack with "cpack --debug --verbose"
  endif()
endif()

if(DEFINED cr8zip)#"-Dcr8zip" as cmake command line parameter
  #include(createZip.cmake)
endif()

if(NOT ${EXE_TYPE} STREQUAL "cre8CmnFilesDebPkg")
  include(${cmnSrcDir}/CMake/writeItemsToFile.cmake)
  Set(srcFilePathsFileName ${EXE_TYPE}_source_file_paths.txt)
  message("Writing source file paths to file \"${srcFilePathsFileName}\".")
  #For debugging purposes
  write1ItemPerLneToFile(${srcFilePathsFileName} "${SOURCE_FILE_PATHS}")
endif()
message("If you didn't explicitely set some of the variables, then they may\
come from a previous call to cmake. If you don't want these settings, delete \
\"CMakeCache.txt\"")
