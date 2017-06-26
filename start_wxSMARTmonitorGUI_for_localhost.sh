#from https://stackoverflow.com/questions/9889938/shell-script-current-directory
BASEDIR=$(dirname $0)
echo $BASEDIR
cd $BASEDIR
./wxSMARTmonitor_GUI svcConnConfFile=localhost.xml
