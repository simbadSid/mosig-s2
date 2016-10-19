#!/bin/bash



# Format the input string to be print into the config file






#--------------------------------------
# Executables
#--------------------------------------
	exec_replaceSubString="./util/replaceSubString.sh"


#----------------------------------------
# User parameters 
#----------------------------------------
	nbrParameters=1

	if [ "$#" -lt $nbrParameters ]; then
		echo "Usage: $0 <String to format>"
		exit
	fi

	str=$1


#----------------------------------------
# Script
#----------------------------------------
	res=$($exec_replaceSubString $str "/" "\/")
	echo $res
