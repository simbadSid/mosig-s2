#!/bin/bash


#----------------------------------------
# User parameters 
#----------------------------------------
	nbrParameters=3

	if [ "$#" -lt $nbrParameters ]; then
		echo "Usage: $0 <initial string> <substring to remove> <substring to write>"
		exit
	fi

	str=$1
	strToRemove=$2
	strToAdd=$3


#----------------------------------------
# Script
#----------------------------------------
echo "${str//$strToRemove/$strToAdd}"
