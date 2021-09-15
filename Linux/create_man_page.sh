#!/bin/bash
#The "source" command is used in this file->needs "bash"?! :
#https://stackoverflow.com/questions/4732200/replacement-for-source-in-sh

numCmdLneArgs=$#

filePathOfthisScript=$(readlink -f $0) #Used in both branches.
if [ $numCmdLneArgs -le 2 ]
then
  srcFile=$cmnSrcDir/OperatingSystem/Linux/SGR_colored_terminal_message.sh
  #Only needed to include the file in this branch.
  source $srcFile #function "printError"
  sourceCmdRtrnCode=$?
  msg="$filePathOfthisScript--ERROR: only $numCmdLneArgs command line
   arguments passed to this script.\nPass: >configuration file paths<
>cfg files install path< >exe file paths<"
  echo "all arguments: $*"
  if [ $sourceCmdRtrnCode -eq 0 ] # <=> "source" command successfully executed
  then
    printError $msg
  else
    echo -e "\033[31mERROR loading file \"$srcFile\"\033[0m"
  fi
  exit 1
fi

dirPathOfThisScript=$(dirname "$filePathOfthisScript")

cp $dirPathOfThisScript/SMARTmonitor.man\
 $dirPathOfThisScript/SMARTmonitor.8

#Config file file system paths prefixed with ".." and separated by ";",
#  e.g. "../config/dataCarrierDefs.xml;../config/SMARTsrvCnnctn.xml"
allCfgFilePaths=$1
#E.g. "/usr/share/SMARTmonitor"
cfgDirPath=$2
#E.g. "/usr/local/bin/wxSMARTmonitor_GUI;/usr/local/bin/SMARTmonitor_service"
allExecFilePaths=$3

##Remove '"' around string ('"' neccessary because it contains ';' characters)
#allCfgFilePaths=$(echo $allCfgFilePaths | tr -d '\"')
echo "$filePathOfthisScript--allCfgFilePaths: $allCfgFilePaths"

fileToInclude=$(dirname "$0")/replaceTextBy_sed.sh
#echo "including file \"$fileToInclude\""

#https://stackoverflow.com/questions/10822790/can-i-call-a-function-of-a-shell-script-from-another-shell-script
source $fileToInclude
sourceCmdRtrnCode=$?
if [ $sourceCmdRtrnCode -eq 0 ] # <=> "source" command successfully executed
then
  allCfgFilePathsLiteral="\${allCfgFilePaths}"
  allExeFilePathsLiteral="\${allExecFilePaths}"
  man_file_path=$dirPathOfThisScript/SMARTmonitor.8

  #CMake separates arrays elements by ";", but newlines are needed to
  # display the paths below each other for better readability.
  #allCfgFilePaths=$(echo $allCfgFilePaths | tr ';' '\n') #Replace ";" with "\n"
  #echo "$filePathOfthisScript--allCfgFilePaths with \"\n\": $allCfgFilePaths"

  #Exchange ".." with file sytem root path:
  escapedFileSysPaths=`echo $cfgDirPath | sed 's,/,\\\/,g'`
  sedReplaceStr="s/\.\./$escapedFileSysPaths/g"
  allCfgFilePaths=`echo $allCfgFilePaths | sed $sedReplaceStr`

  echo "$filePathOfthisScript--replacing \"$allCfgFilePathsLiteral\" in file:"\
    $man_file_path
  replace ${allCfgFilePathsLiteral} $allCfgFilePaths $man_file_path

  echo "replacing \"${allExeFilePathsLiteral}\" by \"$allExecFilePaths\" in"\
    "file:$man_file_path"
  replace ${allExeFilePathsLiteral} $allExecFilePaths $man_file_path
else
  msg="$filePathOfthisScript:ERROR loading file \"$fileToInclude\""
  #printError $msg
  echo -e "\033[31m$msg\033[0m"
fi

