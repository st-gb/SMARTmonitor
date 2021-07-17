dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

cp $dirPathOfThisScript/SMARTmonitor.man\
 $dirPathOfThisScript/SMARTmonitor.8

allCfgFilePaths=$1
echo "allCfgFilePaths: $allCfgFilePaths"

echo "including file $(dirname "$0")/replaceTextBy_sed.sh"

#https://stackoverflow.com/questions/10822790/can-i-call-a-function-of-a-shell-script-from-another-shell-script
source $(dirname "$0")/replaceTextBy_sed.sh

man_file_path=$dirPathOfThisScript/SMARTmonitor.8
echo "replacing \"allCfgFilePaths\" in file:" $man_file_path
replace "allCfgFilePaths" $allCfgFilePaths $dirPathOfThisScript/SMARTmonitor.8

