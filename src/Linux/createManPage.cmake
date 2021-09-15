#${SGR_Cyan},${SGR_ColourReset}
include(${cmnSrcDir}/CMake/SGR_terminalColours.cmake)

message(STATUS "${SGR_Cyan}Creating \"man-page\":begin${SGR_ColourReset}")
message("SMARTmonitor config files in Debian package:" ${allCfgFilePaths})
set(createManPageFilePath
 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Linux/create_man_page.sh)

#For "source" command to work with common_sourcecode path in the shell script
execute_process(COMMAND "export cmnSrcDir=${cmnSrcDir}")

#string(REGEX REPLACE ";$" "\n" allCfgFilePaths "${allCfgFilePaths}")
#get_filename_component(createManPageFilePath ${createManPageFilePath} ABSOLUTE)
#Use "" around varable to get _all_ arrays elements
message("calling \"${createManPageFilePath}\" with parameter(s) "
\""${allCfgFilePaths}"\" ${resourcesFSpath} ${exeFileNames})
#Use "" around varable to get _all_ arrays elements
execute_process(COMMAND ${createManPageFilePath} "${allCfgFilePaths}"
  ${resourcesFSpath} "${exeFileNames}")

set(manFileToCompress ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Linux/SMARTmonitor.8)
#https://wiki.ubuntuusers.de/gzip/ "-k" to keep the the original file (avoid its
# deletion)

#Get/resolve away ".." in file path.
get_filename_component(manFileToCompress "${manFileToCompress}" ABSOLUTE)
#"man gzip" :
# to avoid error: "already exists; do you wish to overwrite (y or n)?"
# "-f --force" : "Force compression [...] even if [...] the corresponding file
#  already exists"
# "-k --keep" : "Keep (don't delete) input files during compression or
#  decompression."
set(cmdLine "gzip --keep --force ${manFileToCompress}")
message("Command line to execute:${cmdLine}")
execute_process(COMMAND ${cmdLine} RESULT_VARIABLE rsltVar)
if( NOT ${rsltVar} EQUAL 0 )
  message(STATUS "${SGR_Red}Calling command \"${cmdLine}\" failed. Return code:"
    "${rsltVar}${SGR_ColourReset}")
endif()

#Store in /usr/share/man/man8 as for "smartctl".
install(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Linux/SMARTmonitor.8.gz
  DESTINATION /usr/share/man/man8)
message(STATUS "${SGR_Cyan}Creating \"man-page\":end${SGR_ColourReset}")

