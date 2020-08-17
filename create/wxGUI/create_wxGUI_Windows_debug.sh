#Gets directory path
dirPathOfThisScript=$(readlink -f $0)
#->absolute directory path
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

bash $dirPathOfThisScript/create_wxGUI_Windows.sh \
"Unix Makefiles" \
"-DwxWidgets_ROOT_DIR=E:/wxWidgets/MSW-3.1.0_gcc510TDM_x64_Dev -Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug"
