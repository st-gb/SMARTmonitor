dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

cp $dirPathOfThisScript/create_.service_file_default_parameters.skeleton\
 $dirPathOfThisScript/create_.service_file_default_parameters.sh

SMARTmonWorkDir=$1
svcExeName=$2

#https://stackoverflow.com/questions/10822790/can-i-call-a-function-of-a-shell-script-from-another-shell-script
source $(dirname "$0")/../replaceTextBy_sed.sh

pathOfFileToChange=$dirPathOfThisScript/create_.service_file_default_parameters.sh
replace "SMARTmonWorkDir" $SMARTmonWorkDir $pathOfFileToChange 
replace "svcExeName" $svcExeName $pathOfFileToChange

