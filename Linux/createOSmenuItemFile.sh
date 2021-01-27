dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

cp $dirPathOfThisScript/SMARTmonitor.desktop.skeleton\
 $dirPathOfThisScript/SMARTmonitor.desktop

exeFilePath=$1
iconFilePath=$2
echo "executable file path to use as replacement:" $exeFilePath

#https://stackoverflow.com/questions/10822790/can-i-call-a-function-of-a-shell-script-from-another-shell-script
source $(dirname "$0")/replaceTextBy_sed.sh

replace "exePath" $exeFilePath $dirPathOfThisScript/SMARTmonitor.desktop
replace "iconPath" $iconFilePath $dirPathOfThisScript/SMARTmonitor.desktop

