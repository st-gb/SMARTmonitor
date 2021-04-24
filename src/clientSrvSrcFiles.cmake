#This file should be used by
#-src/CMakeLists.txt to build the different targets
#-the minimal SMARTmonitorClient derived class

include(${SMARTmonSrcDir}/setCmnSrcDir.cmake)

if(UNIX)
  #Linux (e.g. Lubuntu) has tinyxml2 in its package manager
  #"apt-get install libtinyxml"
endif()
if(TINYXML2_ROOT_PATH STREQUAL "")
  set(TINYXML2_ROOT_PATH ${SMARTmonSrcDir}/../../../tinyxml2-master )
endif()
#include(${cmnSrcDir}/CMake/getAbsPath.cmake)
#getAbsPath(${TINYXML2_ROOT_PATH} "${SMARTmonSrcDir}/../../../tinyxml2-master")
message("TINYXML2_ROOT_PATH: ${TINYXML2_ROOT_PATH}")

if(DEFINED resourcesFSpath)
  message("resourcesFSpath defined")
  set(CXX_DEFINITIONS "${CXX_DEFINITIONS} -DresourcesFSpath=${resourcesFSpath}")
endif()

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

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")#-g3 -gdwarf-2
  #https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -gdwarf-2")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -gdwarf-2")
endif()

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
  #To avoid a lot of warnings like " warning: 'virtual void
  # wxWindowBase::SetInitialBestSize(const wxSize&)' is deprecated: use
  # SetInitialSize() instead. [-Wdeprecated-declarations]"
  # when using MinGW.
  # https://stackoverflow.com/questions/48092812/warning-virtual-void-wxwindowbasesetinitialbasesizeconst-wxsize-is-depre
  # "This is a known issue for MinGW GCC compiler version < 5.3 (related to "assert" implementation)."
  add_definitions(-Wdeprecated-declarations)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdeprecated-declarations")
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

if( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(UNIX)#Address sanitizer/ "asan" library only available in Linux?
    #On Windows: "x86_64-w64-mingw32/bin/ld.exe: cannot find -lasan"
    #https://github.com/google/sanitizers/wiki/AddressSanitizer
    set(CMAKE_CXX_FLAGS # "-fsanitize=address" links to libasan.so
      "${CMAKE_CXX_FLAGS} -fsanitize=address")
    add_definitions(-fsanitize=address)
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    add_definitions(-fno-omit-frame-pointer)
  endif()
endif()

set(InclDirs
  ${SMARTmonSrcDir}
  ${cmnSrcDir} 
  ${TINYXML2_ROOT_PATH}
  )

if(NOT WIN32)
  set(InclDirs ${InclDirs} ${cmnSrcDir}/Controller/MSVC_adaption)#tchar.h etc.
endif()

#Avoid GCC "warning: extended initializer lists only available with -std=c++11
# or -std=gnu++11"
include(${cmnSrcDir}/CMake/useCpp11.cmake)
use_cxx11()
