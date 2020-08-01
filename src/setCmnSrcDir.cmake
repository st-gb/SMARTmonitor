set(cmnSrcDir ${COMMON_SOURCECODE_ROOT_PATH})

set(cmnSrcDirDefined YES)
#TODO make for different variables as "getAbsPath" function in 
# common_sourcecode: getAbsPath(cmnSrcDir "${SMARTmonSrcDir}/../../common_sourcecode")
if(NOT DEFINED cmnSrcDir OR cmnSrcDir STREQUAL "")
  set(cmnSrcDirDefined NO)
  #By default "common_sourcecode" lies/lays at the same level as SMARTmonitor /
  #is located 1 level above ${SMARTmonSrcDir}
  set(cmnSrcDir ${SMARTmonSrcDir}/../../common_sourcecode)
endif()
#https://stackoverflow.com/questions/39027269/cmake-convert-relative-path-to-absolute-path-with-build-directory-as-current-d
get_filename_component(absPath ${cmnSrcDir} ABSOLUTE)
#get_filename_component(absPath "${cmnSrcDir}"
#  REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
set(COMMON_SOURCECODE_ROOT_PATH ${absPath})
set(cmnSrcDir ${absPath})
if(cmnSrcDirDefined)
  message("cmnSrcDir:" ${absPath})
else()
  message("cmnSrcDir empty or not defined->using default path:${cmnSrcDir}")
endif()
