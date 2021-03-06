#This script must have Unix line endings (only '\n'/newline/value 10d) if
# executing under Linux. Else error like "$'\r': Kommando nicht gefunden."
echo "this script file: $0"
#dirPathOfThisScript=$(dirname "$0")
dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
#echo "directory path of this script file:" ${dirPathOfThisScript}

CMakeBuildSysGenerator=$1
AdditionalCMakeOpts=$2
wxSetupHpath=$3
echo "CMakeBuildSysGenerator: \"$CMakeBuildSysGenerator\""
echo "AdditionalCMakeOpts: \"$AdditionalCMakeOpts\""

bash $dirPathOfThisScript/create_wxGUI.sh \
/usr/include/wx-3.0 \
$wxSetupHpath \
$dirPathOfThisScript/../../../../tinyxml2-master \
$dirPathOfThisScript/../../../common_sourcecode \
/usr/bin/gcc \
/usr/bin/g++ \
"$CMakeBuildSysGenerator" \
"$AdditionalCMakeOpts"
