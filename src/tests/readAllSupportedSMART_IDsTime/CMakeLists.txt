project(readAllSupportedSMART_IDsTime)

cmake_minimum_required(VERSION 2.60)#TODO really min. ver.?

if(NOT DEFINED TU_Bln361095SMARTmonSrcRootDir)
  ##Assume SMART monitor root source file system directory to be 2 levels above.
  set(##Use author's prefix because other SMARTmon software (package) may exist.
    TU_Bln361095SMARTmonSrcRootDir ${PROJECT_SOURCE_DIR}/../..)
endif()

#set(TU_Bln361095cmnSrcRootDir ${SMARTmonSrcDir}/../../common_sourcecode)
include(${TU_Bln361095SMARTmonSrcRootDir}/setCmnSrcDir.cmake)

##TU_Bln361095setAndResolveFileSysPath(...)
include(${TU_Bln361095cmnSrcRootDir}/FileSystem/setAndResolveFileSysPath.cmake)
TU_Bln361095setAndResolveFileSysPath(TU_Bln361095SMARTmonSrcRootDir
  ${PROJECT_SOURCE_DIR}/../.. 0)

if(DEFINED TU_Bln361095cxxoptsDir)
  message("TU_Bln361095cxxoptsDir defined, value: ${TU_Bln361095cxxoptsDir}")
else()
  #set(cxxoptsDir ${SMARTmonSrcDir}/../../..)
  ##TU_Bln361095setAndResolveFileSysPath(...)
  TU_Bln361095setAndResolveFileSysPath(TU_Bln361095cxxoptsDir
    ${TU_Bln361095SMARTmonSrcRootDir}/../../.. 0)
  message("TU_Bln361095cxxoptsDir not defined->set to \""
    "${TU_Bln361095cxxoptsDir}\"")
endif()

if(DEFINED TU_Bln361095BoostCppLibRootDir)
  message("\"Boost\" C++ library root directory defined:"
    "\"${TU_Bln361095BoostCppLibRootDir}\"")
  set(inclDirs
    ${inclDirs}
    ${TU_Bln361095BoostCppLibRootDir})
  add_compile_definitions(-DTU_Bln361095useBoostMultiprecisionCppInt)
else()
  message("\"Boost\" C++ library root directory(\"TU_Bln361095boostRootDir\")"
    " NOT defined")
endif()

set(srcFilePaths
  main.cpp
  #TODO Make 3 source files as CMake variable "SMARTattrSrcFiles" and use here?
  # Or all 4 source files into SMARTaccBaseSrcFiles?
  ##For class SMARTaccessBase
  ${TU_Bln361095SMARTmonSrcRootDir}/attributes/SMARTattributeNameAndID.cpp

  ##For SMARTattr::setRawVal(...)
  ${TU_Bln361095SMARTmonSrcRootDir}/attributes/SMARTattrVal.cpp
  ##For class SMARTaccessBase
  ${TU_Bln361095SMARTmonSrcRootDir}/attributes/SMARTuniqueID.cpp

  ##For SMARTaccessBase
  ${TU_Bln361095SMARTmonSrcRootDir}/attributes/SMARTuniqueIDandValues.cpp
  ${TU_Bln361095SMARTmonSrcRootDir}/SMARTaccessBase.cpp

  ##For "TU_Bln361095hardwareDataCarrierNVMeUse(EnSMARTattrNames)", declared as
  ##"const char * TU_Bln361095::hardware::dataCarrier::NVMe::EnSMARTattrNames[]"
  ## in "TU_Bln361095::SMARTmon::OpSys::Windows::GetSuppSMART_IDs(...)"
  ${TU_Bln361095cmnSrcRootDir}/hardware/dataCarrier/NVMe/enSMARTattrNames.cpp
  ##const char * TU_Bln361095hardwareDataCarrierNVMeEnSMARTattrNames[]
  #${TU_Bln361095cmnSrcRootDir}/hardware/dataCarrier/NVMe/enSMARTattrNames.c
  )

if(UNIX)
  include(${TU_Bln361095SMARTmonSrcRootDir}/libATA_SMART/libATA_SMART.cmake)
  set(srcFilePaths ${srcFilePaths} ${directSMARTaccessSrcFilePaths})
endif()

include_directories(${TU_Bln361095SMARTmonSrcRootDir}
  ${TU_Bln361095cmnSrcRootDir} ${TU_Bln361095cxxoptsDir} ${inclDirs})

##TU_Bln361095printCmplrInclDirs(...)
include(${TU_Bln361095cmnSrcRootDir}/compiler/C,C++/printInclDirs.cmake)
TU_Bln361095printCmplrInclDirs(6)
##TU_Bln361095printList1ItemPerLine(...)
include(${TU_Bln361095cmnSrcRootDir}/CMake/printList1ItemPerLine.cmake)
TU_Bln361095printList1ItemPerLine("source file paths:" "${srcFilePaths}")

add_executable(${PROJECT_NAME} ${srcFilePaths})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
