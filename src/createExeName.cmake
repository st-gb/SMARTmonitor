#Used by both CMakeLists.txt and createDebPkg.cmake

function(createExeName EXE_TYPE)
if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(EXE_NAME wx${PROJECT_NAME}_GUI)
endif( ${EXE_TYPE} STREQUAL "wxGUI" )

if( ${EXE_TYPE} STREQUAL "ncursesClient")
  set(EXE_NAME ${PROJECT_NAME}_client_ncurses)
endif( ${EXE_TYPE} STREQUAL "ncursesClient" )

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(EXE_NAME ${PROJECT_NAME}_service)
endif()

set(EXE_NAME ${EXE_NAME}_${CMAKE_BUILD_TYPE})
set(EXE_NAME_WOUT_EXT ${EXE_NAME})
include(${cmnSrcDir}/CMake/addExeFileExt.cmake)
addExeFileExt(EXE_NAME)
#Has to be done to progagate changes to EXE_NAME outside of this function.
set(EXE_NAME ${EXE_NAME} PARENT_SCOPE)
endfunction()