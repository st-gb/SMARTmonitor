set(cmnSrcDir ${COMMON_SOURCECODE_ROOT_PATH})

set(cmnSrcDirDefined YES)
set(TU_Bln361095cmnSrcDirDefined YES)
#TODO make for different variables as "getAbsPath" function in 
# common_sourcecode: getAbsPath(cmnSrcDir "${SMARTmonSrcDir}/../../common_sourcecode")
if(NOT DEFINED TU_Bln361095cmnSrcRootDir OR TU_Bln361095cmnSrcRootDir STREQUAL
  "")
  set(TU_Bln361095cmnSrcDirDefined NO)
  #By default "common_sourcecode" lies/lays at the same level as SMARTmonitor /
  #is located 1 level above ${SMARTmonSrcDir}
  set(TU_Bln361095cmnSrcRootDir
    ${TU_Bln361095SMARTmonSrcDir}/../../common_sourcecode
  )
endif()
#https://stackoverflow.com/questions/39027269/cmake-convert-relative-path-to-absolute-path-with-build-directory-as-current-d
get_filename_component(absFileSysDirPath ${TU_Bln361095cmnSrcRootDir} ABSOLUTE)
#get_filename_component(absPath "${cmnSrcDir}"
#  REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
set(COMMON_SOURCECODE_ROOT_PATH ${absPath})
set(cmnSrcDir ${absPath})
if(cmnSrcDirDefined)
  message("cmnSrcDir:" ${absPath})
else()
  message("TU_Bln361095cmnSrcDir empty or not defined->using default path:"
    "${TU_Bln361095cmnSrcRootDir}")
endif()
