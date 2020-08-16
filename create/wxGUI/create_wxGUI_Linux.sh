echo "this script file: $0"
#dirPathOfThisScript=$(dirname "$0")
dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
#echo "directory path of this script file:" ${dirPathOfThisScript}

CMakeBuildSysGenerator=$1
AdditionalCMakeOpts=$2
echo "CMakeBuildSysGenerator: \"$CMakeBuildSysGenerator\""
echo "AdditionalCMakeOpts: \"$AdditionalCMakeOpts\""

bash $dirPathOfThisScript/create_wxGUI.sh \
/usr/include/wx-3.0 \
/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-3.0 \
/media/sg/devel/Libs/TinyXML2/tinyxml2 \
/media/sg/devel/SourceCodeManagement/common_sourcecode \
/usr/bin/gcc \
/usr/bin/g++ \
"$CMakeBuildSysGenerator" \
"$AdditionalCMakeOpts"
