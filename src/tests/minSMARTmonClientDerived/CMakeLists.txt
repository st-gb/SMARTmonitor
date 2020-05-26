project(minSMARTmonClient)
cmake_minimum_required(VERSION 2.10)#TODO really the min version?

set(SMARTmonSrcDir ../..)

include(${SMARTmonSrcDir}/clientSrvSrcFiles.cmake)

add_definitions(${CXX_DEFINITIONS})

include(${SMARTmonSrcDir}/client/SMARTmonClientDerived.cmake)
set(srcFilePaths
  main.cpp
  ${SOURCE_FILE_PATHS}
)
include_directories(${InclDirs})

include(${cmnSrcDir}/CMake/ShowMsg_CompLvl.cmake)
showInclDirs(5)
#include(${COMMON_SOURCECODE_ROOT_PATH}/CMake/writeItemsToFile.cmake)
#writeItemsToFile(source_file_paths.txt ${srcFilePaths})
show1ItmPerLne(5 "" "${srcFilePaths}")

add_executable(${PROJECT_NAME} ${srcFilePaths})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
