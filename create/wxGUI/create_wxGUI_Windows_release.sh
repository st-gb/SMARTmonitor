echo "Under MS Windows path of \"readlink\" and "dirname" must be in \"PATH\" environment variable"
echo "Usually >>Cygwin root directory <<\bin includes these 2 executables."
exeDir=$3
origPATH=$PATH
PATH=$exeDir:$PATH
echo "PATH:" $PATH
#Gets directory path
dirPathOfThisScript=$(readlink -f $0)
#->absolute directory path
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

#e.g. "E:/wxWidgets/MSW-3.1.0_gcc510TDM_x64_Dev"
wxRootDir=$1
CMakeGen=$2

bash $dirPathOfThisScript/create_wxGUI_Windows.sh \
"$CMakeGen" \
"-DwxWidgets_ROOT_DIR=$wxRootDir -Dmultithread=ON -DCMAKE_BUILD_TYPE=Release"

PATH=$origPATH