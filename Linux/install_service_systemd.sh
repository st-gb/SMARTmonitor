# systemd is Linux-specific: https://en.wikipedia.org/wiki/Systemd
#https://www.freedesktop.org/software/systemd/man/systemd.unit.html

echo "copying SMARTmon.service to /etc/systemd/system/"
cp SMARTmon.service /etc/systemd/system/
echo "enabling SMARTmon.service"
#https://unix.stackexchange.com/questions/47695/how-to-write-startup-script-for-systemd
#install AND start (answered Sep 10 '12 at 15:21 by don_crissti):
#"-now" to start the service (see "man systemctl")
systemctl enable --now SMARTmon.service
