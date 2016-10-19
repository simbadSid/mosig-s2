#!/bin/bash



#----------------------------------------
# User parameters 
#----------------------------------------
	nbrParameters=2

	if [ "$#" -lt $nbrParameters ]; then
		echo "Usage: $0 <file> <string to write>"
		exit
	fi
	file=$1
	stringToWrite=$2


#----------------------------------------
# Script
#----------------------------------------
	tmpFile=".tmp"
	echo $stringToWrite | cat - $file > $tmpFile && mv $tmpFile $file
	rm -f $tmpFile

