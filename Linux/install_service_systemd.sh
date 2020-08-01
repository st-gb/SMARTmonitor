# systemd is Linux-specific: https://en.wikipedia.org/wiki/Systemd
#https://www.freedesktop.org/software/systemd/man/systemd.unit.html

numCmdLineArgs=$#
if [ $numCmdLineArgs -ge 1 ]; then #If at least 1 command line argument
 SMARTmonWorkDir=$1
else
 echo "Pass the directory of the service as command line argument"
 SMARTmonWorkDir=`pwd`
fi

echo "Using \"$SMARTmonWorkDir\" as current working directory for SMART monitor service"

#https://stackoverflow.com/questions/13373984/how-to-replace-a-variable-in-shell-script-string
#https://unix.stackexchange.com/questions/211834/slash-and-backslash-in-sed
#https://stackoverflow.com/questions/2369314/why-does-sed-require-3-backslashes-for-a-regular-backslash
#TODO Make paths like ".." absolute in order to work.
#https://stackoverflow.com/questions/4175264/how-to-retrieve-absolute-path-given-relative
escapedPath=`echo $SMARTmonWorkDir | sed 's,/,\\\/,g'`
echo "sed-escaped path:" $escapedPath
sedReplaceStr="s/SMARTmonWorkDir/$escapedPath/g"
echo "sed replace string:" $sedReplaceStr

#Following has to be executed in the directory of the .skeleton file?
#TODO Only replace if it is not within a comment
sed $sedReplaceStr SMARTmon.service.skeleton > SMARTmon.service

echo "copying SMARTmon.service to /etc/systemd/system/"
cp SMARTmon.service /etc/systemd/system/

echo "enabling SMARTmon.service"
#https://unix.stackexchange.com/questions/47695/how-to-write-startup-script-for-systemd
#install AND start (answered Sep 10 '12 at 15:21 by don_crissti):
#"-now" to start the service (see "man systemctl")
systemctl enable --now SMARTmon.service

#https://github.com/systemd/systemd/issues/3010
#Check state afterwards
#systemctl status SMARTmon
