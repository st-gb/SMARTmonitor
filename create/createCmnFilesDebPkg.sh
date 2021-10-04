#Creates a Debian package with files for configuration and information
#(liability etc.) that are common for (Graphical) User Interface and
#server/service/daemon.
dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

buildType=$1
AdditionalCMakeArgs="-DresourcesFSpath=/usr/share/SMARTmonitor"

#https://tldp.org/LDP/abs/html/comparison-ops.html :If string has zero length
if [ -z $cmnSrcDir ]; then
  cmnSrcDir=$dirPathOfThisScript/../../common_sourcecode
  cmnSrcDir=`realpath $cmnSrcDir` #Resolve path to get it shorter.
  echo "\"cmnSrcDir\" empty in shell->setting environment variable with this"\
    "name to default value:$cmnSrcDir"
  export cmnSrcDir=$cmnSrcDir
fi
cmake \
-DEXE_TYPE:STRING=cre8CmnFilesDebPkg \
$AdditionalCMakeArgs \
.
