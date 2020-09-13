
#"see "cmake -G" for available build system generators. e.g. "Unix Makefiles"
BuildSysGenerator=$1
#COMMON_SOURCECODE_ROOT_PATH=../../common_sourcecode
COMMON_SOURCECODE_ROOT_PATH=$2

#e.g. "-DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/usr/bin/g++"
#If using a non-absolute path when setting the C Compiler causes this:
# http://public.kitware.com/Bug/view.php?id=13756
#So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294
# proposes.
otherCMakeArgs=$3

#"-DdirectSMARTaccess":The service usually needs direct access to S.M.A.R.T.
cmake \
-G "$BuildSysGenerator" \
-DCOMMON_SOURCECODE_ROOT_PATH=$COMMON_SOURCECODE_ROOT_PATH \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DEXE_TYPE:STRING=UNIX_service \
-DdirectSMARTaccess:BOOL=ON \
-Dmultithread:BOOL=ON \
$otherCMakeArgs \
src
