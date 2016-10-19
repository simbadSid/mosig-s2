#!/bin/bash

#----------------------------------------
# Global parameters
#----------------------------------------
	markerOsName="#MARKER_OS_NAME"
	markerPath_fileSystem="#MARKER_PATH_FILESYSTEM"
	markerPath_Kernel="#MARKER_PATH_KERNEL"
	markerFileName_fileSystem="#MARKER_FILENAME_FILESYSTEM"
	markerFileName_kernel="#MARKER_FILENAME_KERNEL"
	markerTransfertProtocol="#MARKER_TRANSFERT_PROTOCOL"
	markerNextServerIp="#MARKER_NEXT_SERVER_IP"
	markerNextServerPort="#MARKER_NEXT_SERVER_PORT" 

	markerRestUri_defaultImage="#MARKER_REST_URI_DEFAULT_IMAGE"
	markerRestUri_privateSession="#MARKER_REST_URI_PRIVATE_SESSION"
	markerRestUri_privateSessionImageSpecific="#MARKER_REST_URI_IMAGE_SPECIFIC_PRIVATE_SESSION" 
	markerRestUri_specificImage="#MARKER_REST_URI_SPECIFIC_IMAGE"


#--------------------------------------
# Executables
#--------------------------------------
	exec_formatEntry="./util/formatConfigEntry.sh"


#----------------------------------------
# User parameters 
#----------------------------------------
	nbrParameters=19

	if [ "$#" -ne $nbrParameters ]; then
		echo "Usage: $0 <pxe/iPxe bootstrap script path (used by transfert server)> <pxe/iPxe bootstrap script path (local copy)> <URI local repo> <OS name> <path kernel> <path file system> <kernel file name> <file system file name> <file transfert protocol> <next server IP> <next server port>"
		exit
	fi

	bootstrapScript_system_mainPage=$1
	bootstrapScript_custom_mainPage=$2

#TODO add the last parameters in  the exit comment
	bootstrapScript_system_privateSession=$3
	bootstrapScript_custom_privateSession=$4

	bootstrapScript_system_privateSession_imageSpecific=$5
	bootstrapScript_custom_privateSession_imageSpecific=$6

	uriLocalRepo_formated=$($exec_formatEntry $7)
	osName=$($exec_formatEntry $8)
	path_kernel=$($exec_formatEntry $9)
	path_fileSystem=$($exec_formatEntry ${10})
	fileName_kernel=$($exec_formatEntry ${11})
	fileName_fileSystem=$($exec_formatEntry ${12})
	transfertProtocol=$($exec_formatEntry ${13})
	nextServerIp=$($exec_formatEntry ${14})
	nextServerPort=$($exec_formatEntry ${15})

#TODO add the last parameters in  the exit comment
	REST_URI_DEFAULT_IMAGE=$($exec_formatEntry ${16})
	REST_URI_PRIVATE_SESSION=$($exec_formatEntry ${17})
	REST_URI_PRIVATE_SESSION_IMAGE_SPECIFIC=$($exec_formatEntry ${18})
	REST_URI_SPECIFIC_IMAGE=$($exec_formatEntry ${19})


#----------------------------------------
# Script
#----------------------------------------
	function replaceInFile
	{
		bootstrapScript_system=$1
		bootstrapScript_custom=$2

		touch $bootstrapScript_system

		> $bootstrapScript_system

		cat $bootstrapScript_custom > $bootstrapScript_system

		sed -i -e "s/${markerOsName}/${osName}/g"		        				        $bootstrapScript_system
		sed -i -e "s/${markerPath_Kernel}/${path_kernel}/g"							$bootstrapScript_system 
		sed -i -e "s/${markerPath_fileSystem}/${path_fileSystem}/g"             				$bootstrapScript_system 
		sed -i -e "s/${markerFileName_kernel}/${fileName_kernel}/g"						$bootstrapScript_system 
		sed -i -e "s/${markerFileName_fileSystem}/${fileName_fileSystem}/g"					$bootstrapScript_system 

		sed -i -e "s/${markerTransfertProtocol}/${transfertProtocol}/g"						$bootstrapScript_system
		sed -i -e "s/${markerNextServerIp}/${nextServerIp}/g"							$bootstrapScript_system
		sed -i -e "s/${markerNextServerPort}/${nextServerPort}/g"						$bootstrapScript_system

		sed -i -e "s/${markerRestUri_defaultImage}/${REST_URI_DEFAULT_IMAGE}/g"               			$bootstrapScript_system
		sed -i -e "s/${markerRestUri_privateSession}/${REST_URI_PRIVATE_SESSION}/g"            			$bootstrapScript_system
		sed -i -e "s/${markerRestUri_privateSessionImageSpecific}/${REST_URI_PRIVATE_SESSION_IMAGE_SPECIFIC}/g"	$bootstrapScript_system
		sed -i -e "s/${markerRestUri_specificImage}/${REST_URI_SPECIFIC_IMAGE}/g"				$bootstrapScript_system
	}

	replaceInFile $bootstrapScript_system_mainPage $bootstrapScript_custom_mainPage
	replaceInFile $bootstrapScript_system_privateSession $bootstrapScript_custom_privateSession
	replaceInFile $bootstrapScript_system_privateSession_imageSpecific $bootstrapScript_custom_privateSession_imageSpecific
