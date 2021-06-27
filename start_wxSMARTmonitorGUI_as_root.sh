#from https://stackoverflow.com/questions/9889938/shell-script-current-directory
BASEDIR=$(dirname $0)
echo $BASEDIR
cd $BASEDIR

#https://unix.stackexchange.com/questions/199891/invalid-mit-magic-cookie-1-key-when-trying-to-run-program-remotely
xhost +local: 

numCmdLineArgs=$#

if [ $numCmdLineArgs -lt 1 ]
then
  path=./wxSMARTmonitor_GUI_Debug.elf
  echo "No argument passed->using default program name \""$path"\""
else
  path=$1
  echo "using passed program name \""$path"\""
fi
sudo $path
