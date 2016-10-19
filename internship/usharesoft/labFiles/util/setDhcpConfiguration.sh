#!/bin/bash

#----------------------------------------
# Global parameters
#----------------------------------------
	markerDhcpServerIp="#MARKER_DHCP_SERVER_IP"
	markerDnsServerIp="#MARKER_DNS_SERVER_IP"
	markerNextServerIp="#MARKER_NEXT_SERVER_IP"
	markerBootstrapFileName="#MARKER_BOOTSTRAP_FILE_NAME"
	markerNetworkInterface="#MARKER_NETWORK_INTERFACE"
	markerRooterIp="#MARKER_ROOTER_IP"
	markerSubnetIp="#MARKER_SUBNET_IP"
	markerSubnetMask="#MARKER_SUBNET_MASK"
	markerAddressRangeMin="#MARKER_ADDRESS_RANGE_MIN"
	markerAddressRangeMax="#MARKER_ADDRESS_RANGE_MAX"


#--------------------------------------
# Executables
#--------------------------------------
	exec_formatEntry="./util/formatConfigEntry.sh"


#----------------------------------------
# User parameters 
#----------------------------------------
	nbrParameters=14

	if [ "$#" -ne $nbrParameters ]; then
		echo "Usage: $0 <dhcpConfigFile_system> <dhcpConfigFile_custom> <dhcpConfigFile_client_system> <dhcpConfigFile_client_custom> <dhcpIp> <dnsIp> <nextServerIp> <bootstrapFileName> <networkInterface> <rooterIp> <subnetIp> <subnetMask> <addressRangeMin> <addressRangeMax>"
		exit
	fi


	dhcpConfigFile_system=$1
	dhcpConfigFile_custom=$2
	dhcpConfigFile_client_system=$3
	dhcpConfigFile_client_custom=$4

	dhcpServerIp=$($exec_formatEntry $5)
	dnsServerIp=$($exec_formatEntry $6)
	nextServerIp=$($exec_formatEntry $7)
	bootstrapFileName=$($exec_formatEntry $8)
	networkInterface=$($exec_formatEntry $9)
	rooterIp=$($exec_formatEntry ${10})
	subnetIp=$($exec_formatEntry ${11})
	subnetMask=$($exec_formatEntry ${12})
	addressRangeMin=$($exec_formatEntry ${13})
	addressRangeMax=$($exec_formatEntry ${14})


#----------------------------------------
# Script
#----------------------------------------
	touch $dhcpConfigFile_system
	touch $dhcpConfigFile_client_system

	>$dhcpConfigFile_system
	>$dhcpConfigFile_client_system

	cat $dhcpConfigFile_custom		> $dhcpConfigFile_system 
	cat $dhcpConfigFile_client_custom	> $dhcpConfigFile_client_system

	sed -i -e "s/${markerDhcpServerIp}/${dhcpServerIp}/g"			$dhcpConfigFile_system
	sed -i -e "s/${markerDnsServerIp}/${dnsServerIp}/g"			$dhcpConfigFile_system
	sed -i -e "s/${markerNextServerIp}/${nextServerIp}/g"			$dhcpConfigFile_system
	sed -i -e "s/${markerBootstrapFileName}/${bootstrapFileName}/g"		$dhcpConfigFile_system

	sed -i -e "s/${markerNetworkInterface}/${networkInterface}/g"		$dhcpConfigFile_client_system
	sed -i -e "s/${markerRooterIp}/${rooterIp}/g"				$dhcpConfigFile_client_system
	sed -i -e "s/${markerSubnetIp}/${subnetIp}/g"				$dhcpConfigFile_client_system
	sed -i -e "s/${markerSubnetMask}/${subnetMask}/g"			$dhcpConfigFile_client_system
	sed -i -e "s/${markerAddressRangeMin}/${addressRangeMin}/g"		$dhcpConfigFile_client_system
	sed -i -e "s/${markerAddressRangeMax}/${addressRangeMax}/g"		$dhcpConfigFile_client_system



