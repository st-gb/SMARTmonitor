
#see http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

#https://stackoverflow.com/questions/15662497/difference-between-cmake-current-source-dir-and-cmake-current-list-dir
#https://cmake.org/cmake/help/v3.0/variable/CMAKE_CURRENT_LIST_DIR.html :
# "Full directory of the listfile currently being processed."
set(thisCMakeScriptFileDir ${CMAKE_CURRENT_LIST_DIR})

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${CMAKE_CURRENT_LIST_DIR}/SMARTtableListCtrl.cpp
  ${CMAKE_CURRENT_LIST_DIR}/SupportedSMARTIDsDialog.cpp
  ${CMAKE_CURRENT_LIST_DIR}/wxSMARTmonitorApp.cpp
  ${CMAKE_CURRENT_LIST_DIR}/wxSMARTmonitorDialog.cpp
  ${CMAKE_CURRENT_LIST_DIR}/wxSMARTmonitorTaskBarIcon.cpp
)

if(DEFINED TU_Bln361095useClntSrv)
 set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${CMAKE_CURRENT_LIST_DIR}/ConnectToServerDialog.cpp
  ${CMAKE_CURRENT_LIST_DIR}/client/SMARTmonDlg_client.cpp
  )
endif()

if(UNIX)
  #Force build date to be updated/date of this build. Update time of main dialog
  # source file->is compiled with "__DATE__ preprocessor marcro at compile time.
  execute_process(COMMAND touch ${CMAKE_CURRENT_LIST_DIR}/wxSMARTmonitorDialog.cpp)
endif()

set(CMakeScriptFilePaths
  ${CMakeScriptFilePaths}
  ${CMAKE_CURRENT_LIST_FILE}#this CMake file
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  #see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
  #Set wxWidgets_USE_DEBUG to "ON" to link against WxWidgets debug libraries.
  set( wxWidgets_USE_DEBUG ON)
endif()
message("wxWidgets_USE_DEBUG: ${wxWidgets_USE_DEBUG}")

#Set to appropriate version--releae, debug, 32/64 bit
#Use wxWidgets >= v.2.9 as it provides an appropriate task bar support
if( NOT DEFINED WXWIDGETS_SETUP_H_PATH )
  message("warning: WXWIDGETS_SETUP_H_PATH is NOT defined->compiling will fail"
   "on Linux it may be like: /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-2.8")
endif( NOT DEFINED WXWIDGETS_SETUP_H_PATH )
if( NOT DEFINED WXWIDGETS_INCLUDE_DIR )
  message("warning: WXWIDGETS_INCLUDE_DIR is NOT defined->compiling will fail"
   "on Linux it may be like: /usr/include/wx-2.8")
else()
  message("WXWIDGETS_INCLUDE_DIR defined: ${WXWIDGETS_INCLUDE_DIR}")
endif( NOT DEFINED WXWIDGETS_INCLUDE_DIR )
  
#include_directories( ${INCLUDE_DIRS} )

if(WIN32)
  #On Linux it is e.g. /usr/include
  #Needed for Windows? Because findwxwidgets doesn't locate the include
  # directory for the common (non-setup) header files.
  set(SetwxPathsManually TRUE)
endif()

message("SetwxPathsManually: ${SetwxPathsManually}")
if(SetwxPathsManually)
  #Only add to "include directories" if manual setup
  set(INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${WXWIDGETS_SETUP_H_PATH}
    ${WXWIDGETS_INCLUDE_DIR}
    )
endif()

message("SetwxVarsManually: ${SetwxVarsManually}")
if(SetwxVarsManually)
#Setting manually makes wxWidgets version changes difficult as the version is
#part of the file name: "wx_baseu-3.0" 
set( LIBRARIES
  ${LIBRARIES}
  )

if(WIN32)
  add_definitions(-D__WXMSW__)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -D__WXMSW__
  )
endif(WIN32)

if(UNIX)
  #add_definitions(-D__WXGTK__)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -D__WXGTK__
  )
  set( LIBRARIES
    ${LIBRARIES}
    #libwx_gtk2ud_core-2.8.a
    #wx_baseud-2.8
    #wx_gtk2ud_adv-2.8
  )
endif(UNIX)
else(SetwxVarsManually)

#If not set, wxWidgets_LIBRARIES includes all/too many wxWidgets libraries after
# "find_package(wxWidgets [...]"
SET(wxWidgets_USE_LIBS core base adv)
if(SetFindwxVarsManually)#=set wx vars and call "find_package(wxWidgets..."
#https://wiki.wxwidgets.org/CMake
set(wxWidgets_CONFIGURATION mswud)
#set(wxWidgets_CONFIGURATION mswu)
#TODO already covered by "SET(wxWidgets_USE_LIBS..."?
find_package(wxWidgets COMPONENTS core base adv REQUIRED)
include(${wxWidgets_USE_FILE})

message("wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")
message (STATUS "wxWidgets type: ${wxWidgets_CONFIGURATION}")
else(SetFindwxVarsManually)
set(debugCallFindwxWidgets TRUE)
include(${COMMON_SOURCECODE_ROOT_PATH}/wxWidgets/callFindwxWidgets.cmake)
endif(SetFindwxVarsManually)
endif(SetwxVarsManually)
