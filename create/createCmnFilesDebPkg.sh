#Creates a Debian package with files for configuration and information
#(liability etc.) that are common for (Graphical) User Interface and
#server/service/daemon.

buildType=$1
AdditionalCMakeArgs="-DresourcesFSpath=/usr/share/SMARTmonitor"

cmake \
-DEXE_TYPE:STRING=cre8CmnFilesDebPkg \
$AdditionalCMakeArgs \
.
