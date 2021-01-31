
#echo "Copying SMARTmon.service to /etc/systemd/system/"
#cp $dirPathOfThisScript/SMARTmon.service /etc/systemd/system/

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
