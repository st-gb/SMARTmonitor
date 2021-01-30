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
set(CPACK_PACKAGE_NAME
  ${EXE_NAME_WOUT_EXT}_${CMAKE_SYSTEM_PROCESSOR}_${Distri}_${CMAKE_BUILD_TYPE})
message("Debian package name:" ${CPACK_PACKAGE_NAME})
#Automatically prefixed with "/usr" -> extracted into "/usr/local/bin"
set(exeInstallDir "local/bin")
install(TARGETS ${EXE_NAME}#target name
  #required
  RUNTIME DESTINATION #local/bin # -> extracted into "/usr/local/bin"
    ${exeInstallDir}
    #${CMAKE_INSTALL_BINDIR}
  )
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
  ${localResourcesFSpath}/config/en/SMARTattrDefs.xml
  ${localResourcesFSpath}/config/dataCarrierDefs.xml
  ${localResourcesFSpath}/config/SMARTsrvCnnctn.xml
  )
message("additional Debian package files:" ${additionalFiles})
#https://stackoverflow.com/questions/5232555/how-to-add-files-to-debian-package-with-cpack
INSTALL(FILES ${additionalFiles} #required
  DESTINATION ${resourcesFSpath}/config )
if(${EXE_TYPE} STREQUAL "wxGUI")
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
else()#service
  if(UNIX)
    include(createServiceFile.cmake)
    INSTALL(FILES ${localResourcesFSpath}/Linux/systemd/SMARTmon.service
      DESTINATION /etc/systemd/system/ )

    set(additionalFiles
      ${localResourcesFSpath}/Linux/systemd/create_.service_file.sh
      #${localResourcesFSpath}/Linux/systemd/install_service_systemd.sh
      ${localResourcesFSpath}/Linux/systemd/SMARTmon.service.skeleton
      )
    INSTALL(FILES ${additionalFiles} #required
      DESTINATION ${resourcesFSpath}/systemd )
  endif()
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
