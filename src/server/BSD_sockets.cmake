
set(BSD_sockets_srcFiles
  ${cmnSrcDir}/OperatingSystem/BSD/socket/initSrv.cpp
  #Use variable "SMARTmonSrcDir" for this include script to be usable outside of
  # the main CMakeLists.txt in (unit) tests, too.
  ${SMARTmonSrcDir}/service/SMARTmonitorService.cpp
  )
