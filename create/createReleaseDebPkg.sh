dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

$dirPathOfThisScript/createDebPkg.sh "Release"
