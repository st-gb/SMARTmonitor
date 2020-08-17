#Gets directory path
dirPathOfThisScript=$(readlink -f $0)
#->absolute directory path
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
echo "dirPathOfThisScript $dirPathOfThisScript"

CMakeBuildSysGenerator=$1
AdditionalCMakeOpts=$2

bash $dirPathOfThisScript/create_wxGUI.sh \
E:/wxWidgets/3.1.0/include \
E:/wxWidgets/MSW-3.1.0_gcc510TDM_x64_Dev/lib/gcc_dll/mswu \
E:/Libs/TinyXML2/tinyxml2 \
E:/SourceCodeManagement/common_sourcecode \
E:/compilers/TDM-GCC-64/bin/gcc.exe \
E:/compilers/TDM-GCC-64/bin/g++.exe \
"$CMakeBuildSysGenerator" \
"$AdditionalCMakeOpts"
