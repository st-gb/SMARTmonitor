#from https://www.digitalocean.com/community/tutorials/how-to-configure-a-linux-service-to-start-automatically-after-a-crash-or-reboot-part-1-practical-examples

#from https://unix.stackexchange.com/questions/18209/detect-init-system-using-the-shell
detect_distro()
{
  # init process is pid 1
  INIT=`ls -l /proc/1/exe`
  if [[ $INIT == *"upstart"* ]]; then
    SYSTEMINITDAEMON=upstart
  elif [[ $INIT == *"systemd"* ]]; then
    SYSTEMINITDAEMON=systemd
  elif [[ $INIT == *"/sbin/init"* ]]; then
    INIT=`/sbin/init --version`
    if [[ $INIT == *"upstart"* ]]; then
      SYSTEMINITDAEMON=upstart
    elif [[ $INIT == *"systemd"* ]]; then
      SYSTEMINITDAEMON=systemd
    fi
  fi

  if [ -z "$SYSTEMINITDAEMON" ]; then
    echo "WARNING: Unknown distribution, assuming defaults - this may fail." >&2
  else
    echo "Init system discovered: $SYSTEMINITDAEMON"
  fi
}

if [ "$#" -eq 0 ]; then
  echo "usage:"
  echo "1st: You may specify the init system (\"upstart\", \"systemd\") else an auto-detection starts"
  echo "2nd: executable path of the service"
else
  if [ "$#" -ge 2 ]; then
    SMART_MONITOR_SERVICE_PATH=$2
  fi
fi
#detect_distro

SMART_MONITOR_SERVICE_PATH=SMARTmonitor_service

#System V:
sudo cp SMARTmonitor_service /usr/sbin
sudo cp SMARmonitor_daemon_init.d_script /etc/init.d/SMARTmonitor_deamon
sudo chmod a+x /etc/init.d/SMARTmonitor_deamon
sudo update-rc.d SMARTmonitor_deamon enable

#Upstart
#if [ $SYSTEMINITDAEMON == upstart ]; then
# /etc/init/>>service<<.conf

# systemd:
#  sudo systemctl enable service.service

