#!/bin/bash






#--------------------------------------
# GLobal parameters
#--------------------------------------
	tmpToEraseFileDir=".toErase"
	fileTransfertListSeparator=";"

	localMirorPath=`cat $tmpToEraseFileDir/env_localMirorMountPath`
	transfertServerRootPathList=`cat $tmpToEraseFileDir/env_transfertServerRootPathList`
	transfertServerRootPathList=$(echo $transfertServerRootPathList | tr ";" "\n")


#--------------------------------------
# Executables
# The servce name variables are env variables set by the script setPxeServer.sh
#--------------------------------------
	exec_dhcp=`cat $tmpToEraseFileDir/env_serviceDhcp`
	exec_dns=`cat $tmpToEraseFileDir/env_serviceDns`
	exec_fileTransfertList=`cat $tmpToEraseFileDir/env_serviceListFileTransfert`
	exec_fileTransfertList=$(echo $exec_fileTransfertList | tr ";" "\n")


# -------------------------------------
# Script
#--------------------------------------
	umount $localMirorPath
	rm -r -f $localMirorPath

	service $exec_dhcp stop
	service $exec_dhcp status

	service $exec_dns stop
	service $exec_dns status

	for exec_fileTransfert in $exec_fileTransfertList
	do
		service $exec_fileTransfert stop
		service $exec_fileTransfert status
	done

	for transfertServerRootPath in $transfertServerRootPathList
	do
		rm -r -f $transfertServerRootPath
	done

	rm -r -f $tmpToEraseFileDir/env_localMirorMountPath
	rm -r -f $tmpToEraseFileDir/env_transfertServerRootPathList
	rm -r -f $tmpToEraseFileDir/env_serviceDhcp
	rm -r -f $tmpToEraseFileDir/env_serviceDns
	rm -r -f $tmpToEraseFileDir/env_serviceListFileTransfert
