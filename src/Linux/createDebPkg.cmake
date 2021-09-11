#https://blog.usejournal.com/creating-debian-packages-cmake-e519a0186e87
#To build the .deb package (in folder where CMakeLists.txt resides): 
#1. $ cmake — build .
#2. $ cpack

#${SGR_Cyan},${SGR_ColourReset}
include(${cmnSrcDir}/CMake/SGR_terminalColours.cmake)

#https://de.wikipedia.org/wiki/Filesystem_Hierarchy_Standard#/usr-Verzeichnisstruktur
# : "/usr/local distributionsunabhängige lokale Hierarchie. Hier kann und soll
# die lokale Systemadministration Programme und Daten ablegen, die von der
# entsprechenden Distribution des jeweiligen Systems unabhängig installiert
# worden sind, wie etwa selbstkompilierte oder unabhängig von der Distribution
# heruntergeladene Programme und Dateien."

message("Executable name:" ${EXE_NAME})
#TODO handle cross-compiling (e.g. 64 bit but compiling for 32 bit)
message("CPU archictecture:" ${CMAKE_SYSTEM_PROCESSOR})
#Define distribution via "-DDistri=>name<"
message("Packaging for Linux distribution:" ${Distri})

#Automatically prefixed with "/usr" -> extracted into "/usr/local/bin"
set(exeInstallDir "local/bin"
  #https://cmake.org/cmake/help/v3.0/module/GNUInstallDirs.html
  #${CMAKE_INSTALL_BINDIR}
  )

include(createExeName.cmake)

function(addExeToPkg exeType)
  message("addExeToPkg--${exeType}")
  createExeName(${exeType})
  set(includedExes ${includedExes} ${exeType} PARENT_SCOPE)
#  list(APPEND includedExes ${exeType} PARENT_SCOPE)
#  message("includedExes: ${includedExes}")
set(exeFileNames ${exeFileNames} /usr/${exeInstallDir}/${EXE_NAME} PARENT_SCOPE)
message("addExeToPkg--copy ${EXE_NAME} to ${exeInstallDir}")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../${EXE_NAME}#target name
  #required
  DESTINATION #local/bin # -> extracted into "/usr/local/bin"
    ${exeInstallDir}
  #Needs execute permissions for all groups in order to also execute as
  # non-root.
  #https://cmake.org/cmake/help/latest/command/install.html
  #"default given permissions OWNER_WRITE, OWNER_READ, GROUP_READ, and
  # WORLD_READ if no PERMISSIONS argument is given."
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
  )
endfunction()

include(${cmnSrcDir}/CMake/delimitStringsByPlusChar.cmake)

if(${EXE_TYPE} STREQUAL "debPkg")#->1 package for all (GUI, service) executables
  addExeToPkg("wxGUI")
  addExeToPkg("UNIX_service")
  message("includedExes: ${includedExes}")

  set(exeTypes "unset")
  delimitStringsByPlusChar("${includedExes}" exeTypes)
  message("exeTypes: ${exeTypes}")
  message("includedExes: ${includedExes}")

  set(CPACK_PACKAGE_NAME ${PROJECT_NAME}_${exeTypes})
  message("CPACK_PACKAGE_NAME:${CPACK_PACKAGE_NAME}")
  set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}_${CMAKE_SYSTEM_PROCESSOR})
  if(DEFINED Distri)
    set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}_${Distri})
  endif()
  set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}_${CMAKE_BUILD_TYPE})
  message("CPACK_PACKAGE_NAME:${CPACK_PACKAGE_NAME}")
else(${EXE_TYPE} STREQUAL "debPkg")
  if(${EXE_TYPE} STREQUAL "cre8CmnFilesDebPkg")
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}_cmnFiles")
  else()
    message("EXE_NAME_WOUT_EXT: \"${EXE_NAME_WOUT_EXT}\"")
    set(CPACK_PACKAGE_NAME "${EXE_NAME_WOUT_EXT}_${CMAKE_SYSTEM_PROCESSOR}")
    if(DEFINED Distri OR ${Distri} NOT STREQUAL "")
      set(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}_${Distri}")
    endif()
    set(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}_${CMAKE_BUILD_TYPE}")
    install(TARGETS ${EXE_NAME}#target name #required
      RUNTIME DESTINATION ${exeInstallDir}
      )
  endif()
endif()
#message("Debian package name:" ${CPACK_PACKAGE_NAME})

message("exeFileNames: ${exeFileNames}")

if(DEFINED resourcesFSpath )
  message("resourcesFSpath defined")
else()
  message("resourcesFSpath NOT defined->using default value")
  #Use /usr/share/SMARTmonitor
  #See https://wiki.ubuntuusers.de/Verzeichnisstruktur/
  set(resourcesFSpath "/usr/share/SMARTmonitor")
endif()
message("resourcesFSpath (pass '-DresourcesFSpath=\">path<\"' to CMake):\
${resourcesFSpath}")

set(localResourcesFSpath ".."
  #${CMAKE_CURRENT_SOURCE_DIR}
  )

#include(cfgFilesDebPkg.cmake)
#if(NOT ${EXE_TYPE} STREQUAL "debPkg")#if not 1 package for all executables
#if(${EXE_TYPE} STREQUAL "cre8CfgFilesDebPkg")
  #To avoid GUI and service .deb having the same file paths->package collision
#  createCfgFilesDebPkg(allCfgFilePaths)
#endif()

#TODO use CMake variable holding configuration file names for both C(++) source
# code and here to keep it consistent? Separate names there via non-printable
# character like \t or \n
set(cfgFilePaths
  ${localResourcesFSpath}/config/dataCarrierDefs.xml
  ${localResourcesFSpath}/config/SMARTsrvCnnctn.xml
  )
set(destFolder ${resourcesFSpath}/config)
#https://stackoverflow.com/questions/5232555/how-to-add-files-to-debian-package-with-cpack
INSTALL(FILES ${cfgFilePaths} #required
  DESTINATION ${destFolder})

set(additionalFiles
  ${localResourcesFSpath}/config/en/SMARTattrDefs.xml
  )
INSTALL(FILES ${additionalFiles} #required
  DESTINATION ${resourcesFSpath}/config/en)

set(allCfgFilePaths
  ${cfgFilePaths}
  ${additionalFiles}
  )

if(${EXE_TYPE} STREQUAL "cre8CmnFilesDebPkg")
  #These files are not executables, so set to empty string.
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "")
else()
  include(${PROJECT_SOURCE_DIR}/Linux/createManPage.cmake)
endif()

set(additionalFiles
  ${localResourcesFSpath}/info/disclaimer_of_liability.txt
  ${localResourcesFSpath}/info/Haftungsausschluss.txt
  )
INSTALL(FILES ${additionalFiles} #required
  DESTINATION ${resourcesFSpath}/info)

message("EXE_TYPE before additional files: ${EXE_TYPE}")
if(${EXE_TYPE} STREQUAL "wxGUI" OR ${EXE_TYPE} STREQUAL "debPkg")
  set(addGUIfiles TRUE)
  message("Should add GUI files")
endif()
if(${EXE_TYPE} STREQUAL "UNIX_service" OR ${EXE_TYPE} STREQUAL "debPkg")
  set(addLinuxSvcFiles TRUE)
  message("Should add UNIX_service files")
endif()
if(${addGUIfiles})
  #Following command has to be done for "createOSmenuItem.cmake"
  createExeName("wxGUI")
  if(WIN32)#use .ico files under Windows
  else()
    include(Linux/createOSmenuItem.cmake)
    set(additionalFiles
      ${localResourcesFSpath}/icons/S.M.A.R.T._OK.xpm
      ${localResourcesFSpath}/icons/S.M.A.R.T._unknown.xpm
      ${localResourcesFSpath}/icons/S.M.A.R.T._warning.xpm
      )
    INSTALL(FILES ${additionalFiles} #required
      DESTINATION ${resourcesFSpath}/icons )
  endif()
endif()
if(${addLinuxSvcFiles})
  #Following command has to be done for "createServiceFile.cmake"
  createExeName("UNIX_service")
  message("adding UNIX_service files")
#  if(UNIX)# UNIX not defined if EXE_TYPE="debPkg"?
#    message("UNIX defined")
  if(systemd)
    message("${SGR_Cyan}systemd defined${SGR_ColourReset}")
    include(${PROJECT_SOURCE_DIR}/Linux/systemd/createServiceFile.cmake)
    INSTALL(FILES ${localResourcesFSpath}/Linux/systemd/SMARTmon.service
      DESTINATION /etc/systemd/system/ )
    set(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}_systemd")
  #else()

    include(${PROJECT_SOURCE_DIR}/Linux/postinst.cmake)

    set(additionalFiles
      ${localResourcesFSpath}/Linux/systemd/create_.service_file.sh
      ${localResourcesFSpath}/Linux/systemd/start_service_systemd.sh
      ${localResourcesFSpath}/Linux/systemd/SMARTmon.service.skeleton
      #${localResourcesFSpath}/Linux/systemd/install_service_default_parameters.sh
      )
    INSTALL(FILES ${additionalFiles} #required
      DESTINATION ${resourcesFSpath}/systemd
      #https://cmake.org/cmake/help/latest/command/install.html
      #"default given permissions OWNER_WRITE, OWNER_READ, GROUP_READ, and
      # WORLD_READ if no PERMISSIONS argument is given."
      #Shell scripts need execute permissions.
      PERMISSIONS OWNER_READ OWNER_EXECUTE)
  endif()
#  endif()
endif()

#wenn install scheitert, verweisen auf /var/log
# text-datei, wie man das auf der Kommandozeile installiert und mit GUI/GDebi
#  sudo dpkg -i ${CPACK_PACKAGE_NAME}.deb.
# echo "return of dpkg:" $? 
#  1 or 2: error
# wenn ~ "Versuch, »/usr/local/SMARTmonitor/S.M.A.R.T._OK.xpm« zu überschreiben, welches auch in Paket smartmonitor 1.0.0 ist":
#  -> "apt-cache search smartmonitor"
#   dpkg --remove smartmonitor

SET(CPACK_GENERATOR "DEB")
#https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Component-Install-With-CPack#building-binary-installers-with-cpack
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SMARTmonitor by it-gebauer.de")

#About CPack Debian generator:
#https://cmake.org/cmake/help/v3.6/module/CPackDeb.html

#Format of package dependencies (">=" etc.):
#https://www.debian.org/doc/debian-policy/ch-relationships.html

#To retrieve dependencies to shared libs etc.:
#https://stackoverflow.com/questions/10052041/how-to-list-library-dependencies-of-a-non-native-binary
#1. $ readelf -d ${EXE_NAME} | grep NEEDED
#https://askubuntu.com/questions/481/how-do-i-find-the-package-that-provides-a-file
if(DEFINED directSMARTaccess)
  message("\"directSMARTaccess\" defined")
  set(libsDependendOn "libatasmart4")
endif()
if(${EXE_TYPE} STREQUAL "wxGUI")
  #Needs this package to avoid file path conflicts between (GUI, server) .deb
  set(libsDependendOn "${libsDependendOn}, SMARTmonitor_cmnFiles")
  #Needs at least wxWidgets 3.0?
  set(libsDependendOn "${libsDependendOn}, libwxbase3.0-0v5")
  if(${Distri} STREQUAL "Lubuntu")
    message("Adding dependent packages for Lubuntu")
    #Package name in Lubuntu is different from Ubuntu (additional "gtk3-").
    set(libsDependendOn "${libsDependendOn}, libwxgtk3.0-gtk3-0v5")
  else()
    # Put CMake variables within "" else a semicolon (;) appears.
    set(libsDependendOn "${libsDependendOn}, libwxgtk3.0-0v5")
  endif()
endif()
if(${EXE_TYPE} STREQUAL "UNIX_service")
  #Needs this package to avoid file path conflicts between (GUI, server) .deb
  set(libsDependendOn "${libsDependendOn}, SMARTmonitor_cmnFiles")
endif()

#Don't colour the libraries but only the rest because there may be no library.
message("${SGR_Cyan}Debian package depends on libraries${SGR_ColourReset}:\
\"${libsDependendOn}\"")
# https://cmake.org/cmake/help/v3.3/module/CPackDeb.html
set(CPACK_DEBIAN_PACKAGE_DEPENDS ${libsDependendOn})
#https://cmake.org/cmake/help/v3.3/module/CPackDeb.html#variable:CPACK_DEBIAN_PACKAGE_SHLIBDEPS
#  CPACK_DEBIAN_PACKAGE_SHLIBDEPS 

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Gebauer, M.Sc.Comp.Sc.")#required

include(${cmnSrcDir}/CMake/getDateAsString.cmake)

#https://cmake.org/cmake/help/v3.3/module/CPack.html
#How package name is created: CPACK_PACKAGE_FILE_NAME:
# "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}."
#.deb packages are for Linux, so we can remove "Linux" from the package name
set(CPACK_SYSTEM_NAME "")

message("CPACK_PACKAGE_NAME:\"${SGR_Cyan}${CPACK_PACKAGE_NAME}\
${SGR_ColourReset}\"")

INCLUDE(CPack)# This must always be last!
message("${SGR_Red}To finally create the Debian package:run \"cpack\"\
${SGR_ColourReset}")
