dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")
#echo "directory path of this script file:" ${dirPathOfThisScript}

bash $dirPathOfThisScript/create_wxGUI_Linux.sh \
"Unix Makefiles" \
"-Dmultithread=ON -DCMAKE_BUILD_TYPE=Debug -DdirectSMARTaccess=ON"
