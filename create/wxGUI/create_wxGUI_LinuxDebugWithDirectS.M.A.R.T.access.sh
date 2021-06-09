#This script must have Unix line endings (only '\n'/newline/value 10d) if
# executing under Linux. Else error like "$'\r': Kommando nicht gefunden."
dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
#echo "directory path of this script file:" ${dirPathOfThisScript}

CMakeBuildGenerator=$1
numCmdLineArgs=$#

#lt = less than
if [ $numCmdLineArgs -lt 1 ]; then
  echo "ERROR: specify CMake build file generator as command line argument"
  echo "Available types (\"cmake -G\"):"
  cmake -G
else
 bash $dirPathOfThisScript/create_wxGUI_Linux_debug.sh \
  "$CMakeBuildGenerator" \
  "-Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug -DdirectSMARTaccess=ON -DresourcesFSpath=/usr/share/SMARTmonitor"
fi
