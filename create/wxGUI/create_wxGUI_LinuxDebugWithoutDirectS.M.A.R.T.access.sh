dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
#echo "directory path of this script file:" ${dirPathOfThisScript}

CMakeBuildGenerator=$1
numCmdLineArgs=$#

if [ $numCmdLineArgs -l 1 ]; then
  echo "ERROR: specify CMake build file generator as command line argument"
  echo "Available types (\"cmake -G\"):"
  cmake -G
else
 bash $dirPathOfThisScript/create_wxGUI_Linux_debug.sh \
  "$CMakeBuildGenerator" \
  "-Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug"
fi
