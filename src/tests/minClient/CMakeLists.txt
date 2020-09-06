project(minClient)

cmake_minimum_required(VERSION 2.10)#TODO really min. ver.?

set(SMARTmonSrcDir ${PROJECT_SOURCE_DIR}/../..)
include(${SMARTmonSrcDir}/setCmnSrcDir.cmake)

set(CmnSrcOS ${cmnSrcDir}/OperatingSystem)
set(srcFilePaths
  main.cpp
  ${cmnSrcDir}/OperatingSystem/BSD/socket/prepCnnctToSrv.c
  ${cmnSrcDir}/OperatingSystem/BSD/socket/socketTimeout.cpp
  ${CmnSrcOS}/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.cpp
  )

if(WIN32)#Windows, also 64 bit
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#https://stackoverflow.com/questions/16141451/the-procedure-entry-point-gxx-personality-v0-could-not-be-located-in-the-dynam
# if contains "__gxx_personality_v0" : "nm libstdc++-6.dll | grep personality"
# linker flags: "-static-libgcc -static-libstdc++"

#If not added then runtime error message "The procedure entry point
# __gxx_personality_v0 could not be located in the dynamic link library 
# libstdc++-6.dll" may show up.
#https://stackoverflow.com/questions/11783932/how-do-i-add-a-linker-or-compile-flag-in-a-cmake-file
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()
endif()

#Set cxxoptsDir via "-DcxxoptsDir=<dir path>" as parameter for CMake program
include_directories(${SMARTmonSrcDir} ${cmnSrcDir} ${cxxoptsDir})

include(${cmnSrcDir}/CMake/ShowMsg_CompLvl.cmake)
showInclDirs(6)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(exeName ${PROJECT_NAME}_dbg)
else()
  set(exeName ${PROJECT_NAME})
endif()
add_executable(${exeName} ${srcFilePaths})

if(WIN32)#Windows, also 64 bit
  target_link_libraries(${exeName} wsock32)
endif()