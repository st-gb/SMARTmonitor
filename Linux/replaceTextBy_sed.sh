#param 1: string in file to replace
#param 2: string to replace param 1 with
#param 3: file path for replacing text in
replace()
{
#https://www.gnu.org/software/bash/manual/html_node/Bash-Variables.html :
# "FUNCNAME" : "The element with index 0 is the name of any currently-executing
# "FUNCNAME" only applies to Bourne Again SHell ("bash")? 
sigOfThisFunction=${FUNCNAME[0]}\(...\)

strToReplace=$1
replaceByStr=$2
filePath=$3
echo "$sigOfThisFunction--string to replace:" \"$strToReplace\"

#https://stackoverflow.com/questions/13373984/how-to-replace-a-variable-in-shell-script-string
#https://unix.stackexchange.com/questions/211834/slash-and-backslash-in-sed
#https://stackoverflow.com/questions/2369314/why-does-sed-require-3-backslashes-for-a-regular-backslash
#TODO Make paths like ".." absolute in order to work.
#https://stackoverflow.com/questions/4175264/how-to-retrieve-absolute-path-given-relative
escapedReplaceBy=`echo $replaceByStr | sed 's,/,\\\/,g'`
echo "sed-escaped string for \"$replaceByStr\":
 $escapedReplaceBy"
sedReplaceStr="s/$strToReplace/$escapedReplaceBy/g"
echo "sed replace string for \"$strToReplace\":
 $sedReplaceStr"

#Following has to be executed in the directory of the .skeleton file?
#TODO Only replace if it is not within a comment
sed -i $sedReplaceStr $filePath
}

