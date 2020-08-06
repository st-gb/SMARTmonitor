#This file should be used by
#-src/CMakeLists.txt to build the different targets
#-the minimal SMARTmonitorClient derived class

include(${SMARTmonSrcDir}/setCmnSrcDir.cmake)

if(TINYXML2_ROOT_PATH STREQUAL "")
  set(TINYXML2_ROOT_PATH ${SMARTmonSrcDir}/../../../tinyxml2-master )
endif()
#include(${cmnSrcDir}/CMake/getAbsPath.cmake)
#getAbsPath(${TINYXML2_ROOT_PATH} "${SMARTmonSrcDir}/../../../tinyxml2-master")
message("TINYXML2_ROOT_PATH: ${TINYXML2_ROOT_PATH}")

#Source files needed for both client/GUI and server builds.
if(DEFINED multithread)
  message("multithread defined")
  set(CXX_DEFINITIONS "${CXX_DEFINITIONS} -Dmultithread")
else()
  message("multithread NOT defined")
endif()
if(DEFINED directSMARTaccess)
  message("directSMARTaccess defined")
  set(CXX_DEFINITIONS "${CXX_DEFINITIONS} -DdirectSMARTaccess")
else()
  message("directSMARTaccess NOT defined")
endif()

set( CXX_DEFINITIONS ${CXX_DEFINITIONS}
  -DUSE_OWN_LOGGER
  -DCOMPILE_WITH_LOG
  -DUNICODE
)

set(ATTRIBUTE_DATA_SOURCE_FILES
  #Make as CMake variable "SMARTattrSrcFiles" ?
  ${SMARTmonSrcDir}/attributes/SMARTattributeNameAndID.cpp
  ${SMARTmonSrcDir}/attributes/SMARTattrDef.cpp
  ${SMARTmonSrcDir}/attributes/SMARTattrDefAccss.cpp
  ${SMARTmonSrcDir}/attributes/SMARTuniqueID.cpp
  ${SMARTmonSrcDir}/attributes/SMARTuniqueIDandValues.cpp
)

if(UNIX)
  message("UNIX defined")
  include(${SMARTmonSrcDir}/Unix.cmake)
endif()
if(WIN32)#https://cmake.org/cmake/help/v3.0/variable/WIN32.html
  message("WIN32 defined")
  include(${SMARTmonSrcDir}/Windows/Windows.cmake)
endif()

set(CmnSrcLogger ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger)
include(${CmnSrcLogger}/defaultLoggerFiles.cmake)

set(CfgRdrSrcfiles
  ${TINYXML2_ROOT_PATH}/tinyxml2.cpp
  ${SMARTmonSrcDir}/tinyxml2/ConfigLoader.cpp
)

set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${defaultLoggerSrcFilePaths}
  ${SMARTmonSrcDir}/UserInterface/UserInterface.cpp
  ${SMARTmonSrcDir}/SMARTmonitorBase.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/stdtstr.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/FileSystem/PathSeperatorChar.cpp
)

if(directSMARTaccess)
  set(CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    #Only needed for direct S.M.A.R.T. access
    ${SMARTmonSrcDir}/SMARTaccessBase.cpp
  )
endif()

if(multithread)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -DCOMPILE_LOGGER_MULTITHREAD_SAFE)
  set(CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    #Only needed for multithreaded. Also for setting the thread name
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/multithread/I_Thread.cpp
    )
endif()

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${CfgRdrSrcfiles}
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${ATTRIBUTE_DATA_SOURCE_FILES}
)

set(InclDirs
  ${SMARTmonSrcDir}
  ${cmnSrcDir} 
  #${cmnSrcDir}/Controller/MSVC_adaption
  ${TINYXML2_ROOT_PATH}
  )
