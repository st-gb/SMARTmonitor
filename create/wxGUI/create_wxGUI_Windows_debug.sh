echo "Under MS Windows path of \"readlink\" and "dirname" must be in \"PATH\" environment variable"
echo "Usually >>Cygwin root directory <<\bin includes these 2 executables."
echo "PATH:" $PATH
#Gets directory path
dirPathOfThisScript=$(readlink -f $0)
echo pathOfthisScript: $dirPathOfThisScript 
#->absolute directory path
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
echo dirPathOfThisScript: $dirPathOfThisScript 

#e.g. "E:/wxWidgets/MSW-3.1.0_gcc510TDM_x64_Dev"
wxRootDir=$1
CMakeGen=$2

#-DSetwxVarsManually=TRUE for adding  _manually_ set wxWidgets' "setup.h" and class header files to include dirs
bash $dirPathOfThisScript/create_wxGUI_Windows.sh \
"Unix Makefiles" \
"-DwxWidgets_ROOT_DIR=$wxRootDir -Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug -DSetwxPathsManually=TRUE"
