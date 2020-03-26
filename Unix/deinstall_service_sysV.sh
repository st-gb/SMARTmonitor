svcName=$1

#See also "man update-rc.d", section "INSTALLING INIT SCRIPT LINKS"

#from https://wiki.ubuntuusers.de/Archiv/SysVinit/#Deaktivieren-von-Diensten:
# Deleting symlinks is not sufficient

# Make stop links from start links. 
sudo update-rc.d $svcName disable
