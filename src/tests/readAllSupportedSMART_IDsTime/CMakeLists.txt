project(readAllSupportedSMART_IDsTime)

cmake_minimum_required(VERSION 2.60)#TODO really min. ver.?

set(SMARTmonSrcDir ${PROJECT_SOURCE_DIR}/../..)
#set(cmnSrcDir ${SMARTmonSrcDir}/../../common_sourcecode)
include(${SMARTmonSrcDir}/setCmnSrcDir.cmake)

set(srcFilePaths
  main.cpp
  #TODO Make 3 source files as CMake variable "SMARTattrSrcFiles" and use here?
  # Or all 4 source files into SMARTaccBaseSrcFiles?
  ${SMARTmonSrcDir}/attributes/SMARTattributeNameAndID.cpp#For SMARTaccessBase
  ${SMARTmonSrcDir}/attributes/SMARTuniqueID.cpp#For class SMARTaccessBase
  ${SMARTmonSrcDir}/attributes/SMARTuniqueIDandValues.cpp#For SMARTaccessBase
  ${SMARTmonSrcDir}/SMARTaccessBase.cpp
  )

if(UNIX)
  include(${SMARTmonSrcDir}/libATA_SMART/libATA_SMART.cmake)
  set(srcFilePaths ${srcFilePaths} ${directSMARTaccessSrcFilePaths})
endif()

include_directories(${SMARTmonSrcDir} ${cmnSrcDir})

include(${cmnSrcDir}/CMake/ShowMsg_CompLvl.cmake)
showInclDirs(6)
show1ItmPerLne(6 "source file paths:" "${srcFilePaths}")

add_executable(${PROJECT_NAME} ${srcFilePaths})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
