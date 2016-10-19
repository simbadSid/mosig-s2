#!/bin/bash









#----------------------------------------
# Executables
#----------------------------------------
	exec_postpendToFile=./postpendToFile.sh


#----------------------------------------
# User parameters 
#----------------------------------------
	nbrParameters=4

	if [ "$#" -ne $nbrParameters ]; then
		echo "Usage: $0 <dnsmasq config file> <interface> <next server IP> <transfert protocol> <transfert server root path>"
		exit
	fi

	dnsmasqConfigFile_system=$1
	interface=$2
	nextServerIp=$3
	transfertFileRootPathFtp=$4


#----------------------------------------
# Dnsmasq parameters
#----------------------------------------
	bindInterface="bind-interfaces"
	domain="pxe.server"
	server="0.0.0.0"
	pxePrompt="\"Press F8 for menu.\", 60"
	pxeService="x86PC, \"Install CentOS 7 from network server $nextServerIp\", pxelinux"


#---------------------------------------
# Script
#---------------------------------------
	touch $dnsmasqConfigFile_system
	> $dnsmasqConfigFile_system
	echo "interface=$interface"				>> $dnsmasqConfigFile_system
	echo "$bindInterface"					>> $dnsmasqConfigFile_system
	echo "domain=$domain"					>> $dnsmasqConfigFile_system
	echo "server=$server"					>> $dnsmasqConfigFile_system
	echo "pxe-prompt=$pxePrompt"				>> $dnsmasqConfigFile_system
	echo "pxe-service=$pxeService"				>> $dnsmasqConfigFile_system
	echo "tftp-root=$transfertFileRootPathFtp"		>> $dnsmasqConfigFile_system
	echo "enable-tftp"					>> $dnsmasqConfigFile_system

