dirPathOfThisScript=$(readlink -f $0)
dirPathOfThisScript=$(dirname "$dirPathOfThisScript")

$dirPathOfThisScript/createServiceViaBuildSystem.sh "Unix Makefiles"
