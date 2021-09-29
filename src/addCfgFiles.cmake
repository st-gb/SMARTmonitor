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

#Used for creating man-page: placeholder in man-page is exchanged by this
# variable content.
set(allCfgFilePaths
  ${cfgFilePaths}
  ${additionalFiles}
  )

set(additionalFiles
  ${localResourcesFSpath}/info/disclaimer_of_liability.txt
  ${localResourcesFSpath}/info/Haftungsausschluss.txt
  )
INSTALL(FILES ${additionalFiles} #required
  DESTINATION ${resourcesFSpath}/info)

