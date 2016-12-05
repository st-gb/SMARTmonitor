
#see http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${CMAKE_CURRENT_SOURCE_DIR}/SMARTtableListCtrl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SupportedSMARTIDsDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/wxSMARTmonitorApp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/wxSMARTmonitorDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/wxSMARTmonitorTaskBarIcon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/wxSMARTvalueProcessor.cpp
 PARENT_SCOPE
)

set(CMAKE_LIST_FILE_PATHS
  ${CMAKE_LIST_FILE_PATHS}
  ${CMAKE_CURRENT_LIST_FILE}
  PARENT_SCOPE
)

#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
#Set wxWidgets_USE_DEBUG to "ON" to link against WxWidgets debug libraries.
set( wxWidgets_USE_DEBUG ON
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE )
message("wxWidgets_USE_DEBUG: ${wxWidgets_USE_DEBUG}")

#Set to appropriate version--releae, debug, 32/64 bit
#Use wxWidgets >= v.2.9 as it provides an appropriate task bar support
set( WXWIDGETS_SETUP_H_HEADER_PATH 
  /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE)
set( WXWIDGETS_HEADER_PATH
  /usr/include/wx-3.0
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE)
  
set(INCLUDE_DIRS
  ${INCLUDE_DIRS}
  ${WXWIDGETS_SETUP_H_HEADER_PATH}
  ${WXWIDGETS_HEADER_PATH}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE)

#include_directories( ${INCLUDE_DIRS} )

#https://wiki.wxwidgets.org/CMake
#set(wxWidgets_CONFIGURATION mswu)
#find_package(wxWidgets COMPONENTS core base REQUIRED)
#include(${wxWidgets_USE_FILE})

set( LIBRARIES
  ${LIBRARIES}
  ${wxWidgets_USE_FILE}
  PARENT_SCOPE )

if (WIN32)
  add_definitions(-D__WXMSW__)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -D__WXMSW__
    PARENT_SCOPE
  )
endif (WIN32)

if (UNIX)
  #add_definitions(-D__WXGTK__)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -D__WXGTK__
    PARENT_SCOPE
  )
  set( LIBRARIES
    ${LIBRARIES}
    #libwx_gtk2ud_core-2.8.a
    #wx_baseud-2.8
    #wx_gtk2ud_adv-2.8
    PARENT_SCOPE
  )
endif (UNIX)

#add_executable(VTrans ${SOURCE_FILE_PATHS})
