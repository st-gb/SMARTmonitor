##Author:Stefan Gebauer(TU Berlin matriculation number 361095)

##${SGR_Yellow},${SGR_Blue},${SGR_ColourReset}
include(${TU_Bln361095cmnSrcRootDir}/CMake/SGR_terminalColours.cmake)

#see http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

#https://stackoverflow.com/questions/15662497/difference-between-cmake-current-source-dir-and-cmake-current-list-dir
#https://cmake.org/cmake/help/v3.0/variable/CMAKE_CURRENT_LIST_DIR.html :
# "Full directory of the listfile currently being processed."
set(thisCMakeScriptFileDir ${CMAKE_CURRENT_LIST_DIR})

##bld=BuiLD:http://www.abbreviations.com/abbreviation/build
##Tm=TiMe:http://www.abbreviations.com/abbreviation/time
##Char=CHARacter:http://www.abbreviations.com/abbreviation/character
##Arr=ARRay:http://www.allacronyms.com/array/abbreviated
##Src=SouRCe:http://www.abbreviations.com/abbreviation/Source
##Fl=FiLe
##Pth=PaTH:http://www.allacronyms.com/path/abbreviated
set(TU_Bln361095bldTmCharArrSrcFlPth ${SMARTmonSrcDir}/bldTmCharArr.c)
set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${TU_Bln361095bldTmCharArrSrcFlPth}
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
  message("${SGR_Yellow}Forcing update of build time character array in \"About"
   "\" dialog by updating timestamp of \"${SGR_Blue}"
   "${TU_Bln361095bldTmCharArrSrcFlPth}${SGR_Yellow}\".${SGR_ColourReset}")
  #Force build date to be updated/date of this build. Update time of main dialog
  # source file->is compiled with "__DATE__ preprocessor marcro at compile time.
  execute_process(COMMAND touch ${TU_Bln361095bldTmCharArrSrcFlPth})
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

if(DEFINED wxLibs)

  message("wxLibs:${wxLibs}")

  ##If setting wxWidgets library file names manually then these file names
  ##should be set on command line and not hardcoded into a CMake file because
  ##the wxWidgets version is part of the file name: "wx_baseu-3.0[...]" and
  ##so this file name may change.
  set( LIBRARIES
    ${LIBRARIES}
  ## Pass "-DwxLibs=>><<" to CMake command (line).
  ##
  ## "_U_nicode release libraries: "u" in file name, for example "libwxbase32u.a"
  ## "_U_nicode _d_ebug" libraries:"ud" in file name,for example "libwxbase32ud.a"
  ## For building Unicode debug version under Microsoft Windows, for example:
  ##  "-DwxLibs=libwxbase32ud.a;libwxmsw32ud_adv.a;libwxmsw32ud_core.a"

  ## For Unicode, pass "_UNICODE" as compiler definition?!, else errors like:
  ## "D:/wxWidgets/3.2.1/include/wx/msw/winundef.h:48:45: error: cannot convert
  ##  'LPCTSTR' {aka 'const wchar_t*'} to 'LPCSTR' {aka 'const char*'}"
    ${wxLibs}
    )

  message("Adding ${wxLibDir} to link directories.")
   ##http://cmake.org/cmake/help/latest/command/link_directories.html :
   ## -"Add directories in which the linker will look for libraries."
   ## -"This command [...] should be avoided"
   ##  alternative: target_link_libraries([...]).
   link_directories(${wxLibDir})

  if(WIN32)##Building under/for Microsoft Windows operating system(also 64 bit)
    add_definitions(-D__WXMSW__
      ##Needed for "wxUSE_UNICODE_WINDOWS_H" to be defined in 
      ## ">>wxWidgets source root directory<</include/wx/msw/winundef.h"
      -D_UNICODE)
    set(CXX_DEFINITIONS
      ${CXX_DEFINITIONS}
      -D__WXMSW__
    )
  endif(WIN32)

  if(UNIX)##Unix(-derived) operating systems like Linux
    #add_definitions(-D__WXGTK__)
    set(CXX_DEFINITIONS
      ${CXX_DEFINITIONS}
      -D__WXGTK__
    )
  endif(UNIX)
else(DEFINED wxLibs)
  ##If not set, wxWidgets_LIBRARIES contains all/too many wxWidgets libraries
  ## after "find_package(wxWidgets [...]".
  SET(wxWidgets_USE_LIBS core base adv)
  ##=Set wxWidgets variables and call "find_package(wxWidgets..."
  if(SetFindwxVarsManually)
    #https://wiki.wxwidgets.org/CMake
    #set(wxWidgets_CONFIGURATION mswud)
    #set(wxWidgets_CONFIGURATION mswu)
    #TODO already covered by "SET(wxWidgets_USE_LIBS..."?
    find_package(wxWidgets COMPONENTS core base adv REQUIRED)
    ##This calls 
    include(${wxWidgets_USE_FILE})

    message("wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")
    message (STATUS "wxWidgets type: ${wxWidgets_CONFIGURATION}")
  else(SetFindwxVarsManually)
    set(debugCallFindwxWidgets TRUE)
    include(${COMMON_SOURCECODE_ROOT_PATH}/wxWidgets/callFindwxWidgets.cmake)
  endif(SetFindwxVarsManually)
endif()#wxLibs
