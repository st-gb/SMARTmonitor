# systemd is Linux-specific: https://en.wikipedia.org/wiki/Systemd
#https://www.freedesktop.org/software/systemd/man/systemd.unit.html

echo "BEGIN $0"

dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

numCmdLineArgs=$#
if [ $numCmdLineArgs -ge 1 ]; then #If at least 1 command line argument
 SMARTmonWorkDir=$1
 if [ $numCmdLineArgs -ge 2 ]; then #If at least 2 command line arguments
  fullExePath=$2
 else
  echo "Pass the name of the service as command line argument,\
 else \"SMARTmonitor_service.elf\" is used."
  fullExePath=$SMARTmonWorkDir/"SMARTmonitor_service.elf"
 fi
else
 echo "Pass the directory of the service as command line argument,\
 else the current working directory is used."
 SMARTmonWorkDir=`pwd`
  echo "Pass the name of the service as 2nd command line argument,\
 else \"SMARTmonitor_service.elf\" is used."
 fullExePath=$SMARTmonWorkDir/"SMARTmonitor_service.elf"
fi

echo "Using \"$SMARTmonWorkDir\" as current working directory for SMART monitor\
 service"
echo "Using \"$fullExePath\" as executable path for SMART monitor service"

cp $dirPathOfThisScript/SMARTmon.service.skeleton\
 $dirPathOfThisScript/SMARTmon.service

#https://stackoverflow.com/questions/10822790/can-i-call-a-function-of-a-shell-script-from-another-shell-script
source $(dirname "$0")/../replaceTextBy_sed.sh

replace "SMARTmonWorkDir" $(realpath "$SMARTmonWorkDir") $dirPathOfThisScript/SMARTmon.service
replace "fullExePath" "$fullExePath" $dirPathOfThisScript/SMARTmon.service

echo "END $0"

