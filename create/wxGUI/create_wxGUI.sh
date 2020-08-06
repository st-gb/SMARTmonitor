#e.g. call this program with 2 arguments: /usr/include/wx-3.0 /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0

#Edit the following variables if needed!
#If using a non-absolute path when setting the C Compiler causes this: http://public.kitware.com/Bug/view.php?id=13756
#So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294 proposes

#change if necessary
NUM_ARGS_NEEDED=4
MINIMAL_NUM_ARGS_NEEDED=3
numCmdLineArgs=$#

#IDEA: pass as args: <NAME=VALUE>, e.g. COMMON_SOURCECODE_ROOT_PATH=../common_sourcecode
if [ $numCmdLineArgs -ge $MINIMAL_NUM_ARGS_NEEDED ]; then
  WXWIDGETS_INCLUDE_DIR=$1
  WXWIDGETS_SETUP_H_PATH=$2
  #Needs to be in brackets because it may consist of > 1 token
  CMakeBuildSysGenerator="$3"
  echo WXWIDGETS_INCLUDE_DIR: $WXWIDGETS_INCLUDE_DIR
  echo WXWIDGETS_SETUP_H_PATH: $WXWIDGETS_SETUP_H_PATH

if [ $numCmdLineArgs -ge 5 ]; then
  TINYXML2_ROOT_PATH=$4
  echo TINYXML2_ROOT_PATH: $TINYXML2_ROOT_PATH
  COMMON_SOURCECODE_ROOT_PATH=$5
fi
if [ $numCmdLineArgs -ge 7 ]; then
  C_COMPILER=$6
  CPP_COMPILER=$7
else
  C_COMPILER=/usr/bin/gcc #default path for Linux and cygwin
  CPP_COMPILER=/usr/bin/g++ #default path for Linux and cygwin
fi
echo CPP_COMPILER: $CPP_COMPILER

# "-pg" option for "gprof" profiling
AdditionalCMakeArgs=$8
echo AdditionalCMakeArgs: $AdditionalCMakeArgs

cmake \
-G "$CMakeBuildSysGenerator" \
-DCMAKE_BUILD_TYPE=Debug \
-DCOMMON_SOURCECODE_ROOT_PATH=$COMMON_SOURCECODE_ROOT_PATH \
-DTINYXML2_ROOT_PATH=$TINYXML2_ROOT_PATH \
-DCMAKE_C_COMPILER=${C_COMPILER} \
-DCMAKE_CXX_COMPILER=${CPP_COMPILER} \
-DCMAKE_C_FLAGS_DEBUG="-g3 -gdwarf-2" \
-DCMAKE_CXX_FLAGS_DEBUG="-g3 -gdwarf-2" \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DEXE_TYPE:STRING=wxGUI \
-DWXWIDGETS_INCLUDE_DIR:STRING=$WXWIDGETS_INCLUDE_DIR \
-DWXWIDGETS_SETUP_H_PATH:STRING=$WXWIDGETS_SETUP_H_PATH \
-DCOMMON_SOURCECODE_ROOT_PATH:STRING=$COMMON_SOURCECODE_ROOT_PATH \
-DTINYXML2_ROOT_PATH:STRING=$TINYXML2_ROOT_PATH \
$AdditionalCMakeArgs \
.
else
  echo "ERROR:at least" $MINIMAL_NUM_ARGS_NEEDED " args needed:"
  echo "specifiy options for this script:<WXWIDGETS_INCLUDE_DIR> <WXWIDGETS_SETUP_H_PATH> <TINYXML2_ROOT_PATH> <COMMON_SOURCECODE_ROOT_PATH>"
  echo "e.g. for 32 bit Ubuntu:"$0 /usr/include/wx-3.0 /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /home/sg/SourceCodeManagement/common_sourcecode /usr/src/tinyxml2-master
  echo "e.g. for 64 bit Linux Mint:"$0 /usr/include/wx-3.0 /usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-3.0 /home/sg/SourceCodeManagement/common_sourcecode /usr/src/tinyxml2-master
fi

