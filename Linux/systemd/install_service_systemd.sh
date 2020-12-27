# systemd is Linux-specific: https://en.wikipedia.org/wiki/Systemd
#https://www.freedesktop.org/software/systemd/man/systemd.unit.html

dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

numCmdLineArgs=$#
if [ $numCmdLineArgs -ge 1 ]; then #If at least 1 command line argument
 SMARTmonWorkDir=$1
 if [ $numCmdLineArgs -ge 2 ]; then #If at least 2 command line arguments
  svcExecName=$2
 else
  echo "Pass the name of the service as command line argument,\
 else \"SMARTmonitor_service.elf\" is used."
  svcExecName="SMARTmonitor_service.elf"
 fi
else
 echo "Pass the directory of the service as command line argument,\
 else the current working directory is used."
 SMARTmonWorkDir=`pwd`
 svcExecName="SMARTmonitor_service.elf"
fi

echo "Using \"$SMARTmonWorkDir\" as current working directory for SMART monitor\
 service"
echo "Using \"$svcExecName\" as executable name for SMART monitor service"

cp $dirPathOfThisScript/SMARTmon.service.skeleton\
 $dirPathOfThisScript/SMARTmon.service

replace()
{
strToReplace=$1
replaceBy=$2
echo "String to replace:" \"$strToReplace\"

#https://stackoverflow.com/questions/13373984/how-to-replace-a-variable-in-shell-script-string
#https://unix.stackexchange.com/questions/211834/slash-and-backslash-in-sed
#https://stackoverflow.com/questions/2369314/why-does-sed-require-3-backslashes-for-a-regular-backslash
#TODO Make paths like ".." absolute in order to work.
#https://stackoverflow.com/questions/4175264/how-to-retrieve-absolute-path-given-relative
escapedReplaceBy=`echo $replaceBy | sed 's,/,\\\/,g'`
echo "sed-escaped string for \"$replaceBy\":
 $escapedReplaceBy"
sedReplaceStr="s/$strToReplace/$escapedReplaceBy/g"
echo "sed replace string for \"$strToReplace\":
 $sedReplaceStr"

#Following has to be executed in the directory of the .skeleton file?
#TODO Only replace if it is not within a comment
sed -i $sedReplaceStr $dirPathOfThisScript/SMARTmon.service
}

replace "SMARTmonWorkDir" $(realpath "$SMARTmonWorkDir")
replace "svcExecName" "$svcExecName"

echo "Copying SMARTmon.service to /etc/systemd/system/"
cp $dirPathOfThisScript/SMARTmon.service /etc/systemd/system/

echo "Enabling and starting SMARTmon.service"
#https://unix.stackexchange.com/questions/47695/how-to-write-startup-script-for-systemd
#install AND start (answered Sep 10 '12 at 15:21 by don_crissti):
#"-now" to start the service (see "man systemctl")
systemctl enable --now SMARTmon.service
sysctlRetCode=$?
echo "Return code for enabling and starting service via \"systemctl\":\
 $sysctlRetCode"

if [ $sysctlRetCode -ne 0 ]; then
 echo "Error enabling and starting the service via \"systemctl\"."
#https://github.com/systemd/systemd/issues/3010
#Check state afterwards
systemctl status SMARTmon.service
fi
