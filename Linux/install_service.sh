#!/bin/bash
# "[[" needs bash

#from https://www.digitalocean.com/community/tutorials/how-to-configure-a-linux-service-to-start-automatically-after-a-crash-or-reboot-part-1-practical-examples

# https://en.wikipedia.org/wiki/Linux_startup_process#Init_process
# Google: "detect init type Linux"
# https://linuxconfig.org/detecting-which-system-manager-is-running-on-linux-system
#https://unix.stackexchange.com/questions/106656/how-do-services-in-debian-work-and-how-can-i-manage-them
# answered Dec 26 '13 at 14:32:  SysVinit, Systemd, Upstart

if [ $# -le 2 ]; then
#http://stackoverflow.com/questions/59895/how-can-i-get-the-source-directory-of-a-bash-script-from-within-the-script-itsel:
# "edited Jan 20 at 2:15 community wiki 4 revs, 3 users 87% SpoonMeiser"
#"[...] dirname $0 should give you the directory of the current script [...]"
  prefix=$(dirname "$0")/../../common_sourcecode
else
  prefix=$3
fi;
fileToIncl=$prefix/OperatingSystem/Linux/coloured_terminal_message.sh
#"source": import/include functions from shell script
#Shell script contains functions: printError(),printSuccess()
source $fileToIncl
if [ $? -ge 1 ]; then
#http://www.tutorialspoint.com/how-to-output-colored-text-to-a-linux-terminal
# "Red 31"
# "Reset 0 Back to normal (remove all styles)"
  echo -e "\033[31mfailed to include file \""$fileToIncl"\"\033[0m"
fi

#from Ben McCann Mar 13 '16 at 4:43
#https://unix.stackexchange.com/questions/18209/detect-init-system-using-the-shell
detect_distro()
{
  lsProc1ExeCmd="ls -l /proc/1/exe"
  # init process is pid 1
  INIT=`$lsProc1ExeCmd`
  proc1ExeRtrnCode=$?
  #Error "ls: cannot read symbolic link '/proc/1/exe': Permission denied"
  # happened when calling this script as normal user in "Arch Linux",
  # -kernel release: "5.11.2-1-ARCH"
  # -kernel version: "#1 SMP PREEMPT Sun Feb 28 22:03:25 UTC 2021"
  # -machine: "armv7l"
  if [ $proc1ExeRtrnCode -gt 0 ]; then
    errorMsg=`echo "Calling \"$lsProc1ExeCmd\" failed: return code=\
$proc1ExeRtrnCode\nRunning this shell script via \"sudo\"/as root/\
administrator may solve this problem."`
    printError $errorMsg
  fi;
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
    printSuccess "Init system discovered: \"$SYSTEMINITDAEMON\""
  fi
}

if [ "$#" -eq 0 ]; then
  echo "No command line arguments specified--thus showing usage:"
  echo "1st command line argument: the init system (\"upstart\", \"systemd\") \
else an auto-detection starts"
  echo "2nd command line argument: file system path of service executable"
  echo "3rd command line argument: file system path to \"common_sourcecode\" \
git repository from Stefan Gebauer"
else
  if [ "$#" -ge 2 ]; then
    SMART_MONITOR_SERVICE_PATH=$2
  fi
fi
detect_distro

SMART_MONITOR_SERVICE_PATH=SMARTmonitor_service

# https://tldp.org/LDP/abs/html/comparison-ops.html :
# if string has zero length.
if [ -z "$SYSTEMINITDAEMON" ]; then
 echo ""
else
#TODO change to "case"?: case "$SYSTEMINITDAEMON" in SystemV)
if [ $SYSTEMINITDAEMON = "SystemV" ]; then
  install_service_sysV.sh
fi;

#Upstart
#if [ $SYSTEMINITDAEMON == upstart ]; then
# /etc/init/>>service<<.conf

if [ $SYSTEMINITDAEMON = "systemd" ]; then
  ../Linux/systemd/install_service_default_parameters.sh
fi;
fi;


