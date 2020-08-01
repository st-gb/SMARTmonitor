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

include_directories(${SMARTmonSrcDir} ${cmnSrcDir} )

include(${cmnSrcDir}/CMake/ShowMsg_CompLvl.cmake)
showInclDirs(6)

add_executable(${PROJECT_NAME} ${srcFilePaths})