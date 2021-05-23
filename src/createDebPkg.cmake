#https://blog.usejournal.com/creating-debian-packages-cmake-e519a0186e87
#To build the .deb package (in folder where CMakeLists.txt resides): 
#1. $ cmake — build .
#2. $ cpack

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

if(${EXE_TYPE} STREQUAL "debPkg")
  addExeToPkg("wxGUI")
  addExeToPkg("UNIX_service")
  message("includedExes: ${includedExes}")

  #from https://stackoverflow.com/questions/17666003/cmake-output-a-list-with-delimiters
  # "," not possible: dpkg: "error processing archive 
  # /media/sg/devel1/SourceCodeManagement/wxSMARTmonitor/src/SMARTmonitor_wxGUI,
  # UNIX_service_x86_64_Lubuntu_Release-1.0.0-Linux.deb (--install):
  # parsing file '/var/lib/dpkg/tmp.ci/control' near line 1:
  # invalid package name (character ',' not allowed (only letters, digits and
  # characters '-+._'))"
  string(REPLACE ";" "+" exeTypes "${includedExes}")
#  foreach(includedExe IN ${includedExes})
#    message("includedExe: " ${includedExe})
#    set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}${includedExe},)
#  endforeach()
set(CPACK_PACKAGE_NAME
  ${PROJECT_NAME}_${exeTypes})
  set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}_${CMAKE_SYSTEM_PROCESSOR}_${Distri}_${CMAKE_BUILD_TYPE})
else(${EXE_TYPE} STREQUAL "debPkg")
  set(CPACK_PACKAGE_NAME
    ${EXE_NAME_WOUT_EXT}_${CMAKE_SYSTEM_PROCESSOR}_${Distri}_${CMAKE_BUILD_TYPE})
  install(TARGETS ${EXE_NAME}#target name #required
    RUNTIME DESTINATION ${exeInstallDir}
    )
endif()
message("Debian package name:" ${CPACK_PACKAGE_NAME})

if(DEFINED resourcesFSpath )
  message("resourcesFSpath defined")
else()
  message("resourcesFSpath NOT defined->using default value")
  #Use /usr/share/SMARTmonitor
  #See https://wiki.ubuntuusers.de/Verzeichnisstruktur/
  set(resourcesFSpath "/usr/share/SMARTmonitor")
endif()
message("resourcesFSpath (pass -DresourcesFSpath=\">path<\" to CMake): ${resourcesFSpath}")

set(localResourcesFSpath ".."
  #${CMAKE_CURRENT_SOURCE_DIR}
  )

#TODO use CMake variable holding configuration file names for both C(++) source
# code and here to keep it consistent? Separate names there via non-printable
# character like \t or \n
set(additionalFiles
  ${localResourcesFSpath}/config/dataCarrierDefs.xml
  ${localResourcesFSpath}/config/SMARTsrvCnnctn.xml
  )
message("additional Debian package files:" ${additionalFiles})
#https://stackoverflow.com/questions/5232555/how-to-add-files-to-debian-package-with-cpack
INSTALL(FILES ${additionalFiles} #required
  DESTINATION ${resourcesFSpath}/config )

set(additionalFiles
  ${localResourcesFSpath}/config/en/SMARTattrDefs.xml
  )
INSTALL(FILES ${additionalFiles} #required
  DESTINATION ${resourcesFSpath}/config/en )

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
    include(createOSmenuItem.cmake)
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
    include(createServiceFile.cmake)
    INSTALL(FILES ${localResourcesFSpath}/Linux/systemd/SMARTmon.service
      DESTINATION /etc/systemd/system/ )

    set(additionalFiles
      ${localResourcesFSpath}/Linux/systemd/create_.service_file.sh
      ${localResourcesFSpath}/Linux/systemd/start_service_systemd.sh
      ${localResourcesFSpath}/Linux/systemd/SMARTmon.service.skeleton
      )
    INSTALL(FILES ${additionalFiles} #required
      DESTINATION ${resourcesFSpath}/systemd
      #https://cmake.org/cmake/help/latest/command/install.html
      #"default given permissions OWNER_WRITE, OWNER_READ, GROUP_READ, and
      # WORLD_READ if no PERMISSIONS argument is given."
      #Shell scripts need execute permissions.
      PERMISSIONS OWNER_READ OWNER_EXECUTE)
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
#2. $ dpkg -S >library name from readelf<
if(DEFINED directSMARTaccess)
  set(libsDependendOn "libatasmart4")
endif()
if(${EXE_TYPE} STREQUAL "wxGUI")
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
message("Debian package depends on libraries:" ${libsDependendOn})
# https://cmake.org/cmake/help/v3.3/module/CPackDeb.html
set(CPACK_DEBIAN_PACKAGE_DEPENDS ${libsDependendOn})
#https://cmake.org/cmake/help/v3.3/module/CPackDeb.html#variable:CPACK_DEBIAN_PACKAGE_SHLIBDEPS
#  CPACK_DEBIAN_PACKAGE_SHLIBDEPS 

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Gebauer, M.Sc.Comp.Sc.")#required
INCLUDE(CPack)# This must always be last!
message("Create Debian package running \"cpack\"")
