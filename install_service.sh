#!/bin/bash
# "[[" needs bash

#from https://www.digitalocean.com/community/tutorials/how-to-configure-a-linux-service-to-start-automatically-after-a-crash-or-reboot-part-1-practical-examples

#https://unix.stackexchange.com/questions/106656/how-do-services-in-debian-work-and-how-can-i-manage-them
# answered Dec 26 '13 at 14:32:  SysVinit, Systemd, Upstart

#from Ben McCann Mar 13 '16 at 4:43
#https://unix.stackexchange.com/questions/18209/detect-init-system-using-the-shell
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

detect_distro

SMART_MONITOR_SERVICE_PATH=SMARTmonitor_service

#TODO change to "case"?: case "$SYSTEMINITDAEMON" in SystemV)
if [ $SYSTEMINITDAEMON = "SystemV" ]; then
  install_service_sysV.sh
fi;

#Upstart
#if [ $SYSTEMINITDAEMON == upstart ]; then
# /etc/init/>>service<<.conf

if [ $SYSTEMINITDAEMON = "systemd" ]; then
  ../Linux/install_service_systemd.sh
fi;

