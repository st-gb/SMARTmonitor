buildType=$1
#service needs direct S.M.A.R.T. access, so add it
AdditionalCMakeArgs="-DresourcesFSpath=/usr/share/SMARTmonitor -Dmultithread=ON -DdirectSMARTaccess=ON"

cmake \
-DEXE_TYPE:STRING=debPkg \
-DexePaths:STRING=SMARTmonitor_service_$buildType.elf \
-DCMAKE_BUILD_TYPE="$buildType" \
$AdditionalCMakeArgs \
.
