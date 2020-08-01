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

install(TARGETS ${EXE_NAME}#target name
  #required
  RUNTIME DESTINATION local/bin # -> extracted into "/usr/local/bin"
    #${CMAKE_INSTALL_BINDIR}
  )
if(${EXE_TYPE} STREQUAL "wxGUI")
  set(additionalFiles
    ../SMARTmonitor.xml
    ../S.M.A.R.T._OK.xpm
    ../S.M.A.R.T._unknown.xpm
    ../S.M.A.R.T._warning.xpm
    )
else()
  set(additionalFiles
    ../SMARTmonitor.xml
    )
endif()
#https://stackoverflow.com/questions/5232555/how-to-add-files-to-debian-package-with-cpack
INSTALL(FILES ${additionalFiles}
  #required
  DESTINATION local/SMARTmonitor # -> extracted into "/usr/local/SMARTmonitor"
    #${CMAKE_INSTALL_BINDIR}
  )
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
  set(libsDependendOn "${libsDependendOn}, libwxgtk3.0-0v5, libwxbase3.0-0v5")
endif()
set(CPACK_DEBIAN_PACKAGE_DEPENDS ${libsDependendOn})

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Gebauer, M.Sc.Comp.Sc.")#required
INCLUDE(CPack)# This must always be last!
