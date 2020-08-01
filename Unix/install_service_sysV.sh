#System V:
#Make soft or hard links. Alternative: copy (cp)
#"ln": make links. with "-s": make soft/symbolic links
ln -s SMARTmonitor_service /usr/sbin/SMARTmonitor_service
ln -s SMARmonitor_daemon_init.d_script /etc/init.d/SMARTmonitor_deamon
chmod a+x /etc/init.d/SMARTmonitor_deamon

#"man update-rc.d" section "INSTALLING INIT SCRIPT LINKS":
#Uses information from init.d script LSB comment header.
sudo update-rc.d SMARTmonitor_deamon enable
