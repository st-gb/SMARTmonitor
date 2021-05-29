buildType=$1
AdditionalCMakeArgs="-DresourcesFSpath=/usr/share/SMARTmonitor -Dmultithread=ON -DdirectSMARTaccess=ON"

cmake \
-DEXE_TYPE:STRING=debPkg \
-DexePaths:STRING=SMARTmonitor_service_$buildType.elf \
-DCMAKE_BUILD_TYPE="$buildType" \
$AdditionalCMakeArgs \
.
