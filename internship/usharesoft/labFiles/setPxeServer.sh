#!/bin/bash





##########################################################################
#
# General notes:
#
#	- Has been tested with FTP for all the transfert steps
#	- The kernel and boot loader files are always put in the boot root directory
#		and transfered using ftp (typically /var/lib/ftpboot/<client os name>)
#	- The client os files are always put in the root directory of the transfert
#		server (typically /var/ftp/<os name> for ftp protocol)
#
##########################################################################









#--------------------------------------
# GLobal parameters
#--------------------------------------
	baseTransfertProtocol="ftp"

	bootDirPath_ftp=/var/ftp
	bootDirPath_http=/usr/share/nginx/html
	bootDirPath_base=bootDirPath_${baseTransfertProtocol}
	bootDirPath_base=${!bootDirPath_base}
	syslinuxDir=/usr/share/syslinux

	bootstrapBin_custom_path=config/pxeConf.d
	bootstrapBin_fileName_isIPxe_1="/undionly.kpxe"
	bootstrapBin_fileName_isIPxe_0="/pxelinux.0"

	bootstrapScript_fileName_mainPage_isIPxe_1="bootstrapScript.mainPage.ipxe"
	bootstrapScript_fileName_mainPage_isIPxe_0="bootstrapScript.mainPage.pxe"

	bootstrapScript_fileName_privateSession="bootstrapScript.privateSession.ipxe"
	bootstrapScript_fileName_privateSession_imageSpecific="bootstrapScript.privateSessionImageSpecific.ipxe"

	pxeProtocol_isIPxe_1="iPxe"
	pxeProtocol_isIPxe_0="pxe"

	defaultFileName_kernel="vmlinuz"
	defaultFileName_fileSystem="initrd.img"

	nextServerPort_ftp="21"
	nextServerPort_http="8080"

	ipxeTransfertProtocolList=( "http" )

	fileTransfertListSeparator=";"
	tmpToEraseFileDir=.toErase

	pathLocalOsIso=../isoFiles

	dnsmasqConfigFile_system=/etc/dnsmasq.conf

	dhcpConfigFile_system=/etc/dhcp/dhcpd.conf
	dhcpConfigFile_custom=./config/dhcpConf.d/dhcpd.conf
	dhcpConfigFile_client_system=/etc/dhcp/conf.d/pxe-network.conf
	dhcpConfigFile_client_custom=./config/dhcpConf.d/pxe-network.conf



# -------------------------------------
# Script options
#--------------------------------------
	parameterNetworkInterface="-networkInterface="
	parameterIp="-ip="
	parameterNextServerIp="-nextServerIp="
	parameterNextServerPort="-nextServerPort="
	parameterTransfertProtocol="-transfertProtocol="
	parameterNetworkIpRange="-networkIpRange="
	parameterGatewayIp="-gatewayIp="
	parameterDnsIp="-dnsIp="
	parameterNoIsoMount="-noIsoMount"
	parameterNoMirorInstall="-noMirorInstall"
	parameterSubnetIp="-subnetIp"
	parameterSubnetMask="-subnetMask="
	parameterAddressRangeMin="-addressRangeMin="
	parameterAddressRangeMax="-addressRangeMax="
	parameterDefaultOs="-defaultOs="
	parameterHelp="-help"


#--------------------------------------
# Executables
#--------------------------------------
	exec_setDnsmasqConfiguration="./util/setDnsmasqConfiguration.sh"
	exec_setDhcpConfiguration="./util/setDhcpConfiguration.sh"
	exec_setPxeConfig="./util/setPxeConfiguration.sh"
	exec_setBootstrapScript="./util/setBootstrapScript.sh"
	exec_newLine="./util/newLine.sh"

	exec_dhcp="dhcpd"
	exec_dns="dnsmasq"
	exec_ftp="vsftpd"
	exec_http="nginx"
	exec_extraTransfert=""
	exec_base=exec_${baseTransfertProtocol}
	exec_base=${!exec_base}


#--------------------------------------
# User input
#--------------------------------------
	# Default parameter values
	networkInterface="eth1"
	ip="192.168.0.1"
	nextServerIp=$ip
	transfertProtocol="http"
	subnetMask="255.255.255.0"
	subnetIp="192.168.0.0"
	addressRangeMin="192.168.0.7"
	addressRangeMax="192.168.0.254"
	defaultOs="centos7_x86_uforge"
	isoMount="1"
	mirorInstall="1"

	function usage
	{
		echo "Usage: $0 [options]"
		echo "Options:"
		echo "    $parameterNetworkInterface<networkInterface>"
		echo "    $parameterIp<ip>"
		echo "    $parameterNextServerIp<ip>"
		echo "    $parameterNextServerPort<port>"
		echo "    $parameterTransfertProtocol<protocol>"
		echo "    $parameterSubnetIp<IP>"
		echo "    $parameterSubnetMask<mask>"
		echo "    $parameterAddressRangeMin<address>"
		echo "    $parameterAddressRangeMax<address>"
		echo "    $parameterNoIsoMount"
		echo "    $parameterNoMirorInstall"
		echo "    $parameterDefaultOs"
		echo "    $parameterHelp"
	}

	# $1: OS name
	# $2: OS iso path
	function checkKnownOs
	{
		if [ ! -f $2 ]; then
			echo "**** Can't find local iso file of for the OS: $1"
			echo "**** Os checked path: $2"
			exit
		fi
	}

	for parameter in $@ ; do
		if [[ $parameter == $parameterNetworkInterface* ]]; then
			networkInterface=${parameter#$parameterNetworkInterface}
		elif [[ $parameter == $parameterIp* ]]; then
			ip=${parameter#$parameterIp}
		elif [[ $parameter == $parameterNextServerIp* ]]; then
			nextServerIp=${parameter#$parameterNextServerIp}
		elif [[ $parameter == $parameterNextServerPort* ]]; then
			nextServerPort=${parameter#$parameterNextServerPort}
		elif [[ $parameter == $parameterTransfertProtocol* ]]; then
			transfertProtocol=${parameter#$parameterTransfertProtocol}
		elif [[ $parameter == $parameterSubnetIp* ]]; then
			subnetIp=${parameter#$parameterSubnetIp}
		elif [[ $parameter == $parameterSubnetMask* ]]; then
			subnetMask=${parameter#$parameterSubnetMask}
		elif [[ $parameter == $parameterAddressRangeMin* ]]; then
			addressRangeMin=${parameter#$parameterAddressRangeMin}
		elif [[ $parameter == $parameterAddressRangeMax* ]]; then
			addressRangeMax=${parameter#$parameterAddressRangeMax}
		elif [[ $parameter == $parameterDefaultOs* ]]; then
			defaultOs=${parameter#$parameterDefaultOs}
			defaultLocalOsIso="$pathLocalOsIso/$defaultOs.iso"
			checkKnownOs $defaultOs $defaultLocalOsIso
		elif [[ $parameter == $parameterHelp ]]; then
			usage
			exit
		elif [[ $parameter == $parameterNoIsoMount ]]; then
			isoMount=0
		elif [[ $parameter == $parameterNoMirorInstall ]]; then
			mirorInstall=0
		else
			echo "Unknown parameter $parameter"
			usage
			exit
		fi
	done

	# CHeck whether to use iPxe or not
	isIPxe=0
	for protocol in "${ipxeTransfertProtocolList[@]}"
	do
		if [ "$protocol" == "$transfertProtocol" ]; then
			isIPxe=1
			break
		fi
	done

	fileTransfertServiceList=$exec_base
	transfertServerRootPathList=$bootDirPath_base
	if [ "$transfertProtocol" != $baseTransfertProtocol ]; then
		exec_extraTransfert=exec_$transfertProtocol
		exec_extraTransfert=${!exec_extraTransfert}
		fileTransfertServiceList=$fileTransfertServiceList$fileTransfertListSeparator$exec_extraTransfert

		transfertServerRootPath=bootDirPath_$transfertProtocol
		transfertServerRootPath=${!transfertServerRootPath}
		transfertServerRootPathList=$transfertServerRootPathList$fileTransfertListSeparator$transfertServerRootPath
	fi


#--------------------------------------
# GLobal parameters
#--------------------------------------
	bootDirPath=bootDirPath_$transfertProtocol
	bootDirPath=${!bootDirPath}
	mkdir $bootDirPath

	pxeProtocol=pxeProtocol_isIPxe_$isIPxe
	pxeProtocol=${!pxeProtocol} 

	if [ "$nextServerPort" = "" ]; then
		nextServerPort=nextServerPort_$transfertProtocol
		nextServerPort=${!nextServerPort}
	fi

	bootstrapBin_fileName=bootstrapBin_fileName_isIPxe_$isIPxe
	bootstrapBin_fileName=${!bootstrapBin_fileName}

	mkdir "$bootDirPath/pxelinux.cfg"

	bootstrapScript_fileName_mainPage=bootstrapScript_fileName_mainPage_isIPxe_$isIPxe
	bootstrapScript_fileName_mainPage=${!bootstrapScript_fileName_mainPage}
	bootstrapScript_custom_mainPage="config/pxeConf.d/script_nonUforge/$bootstrapScript_fileName_mainPage"
	bootstrapScript_system_mainPage="$bootDirPath/pxelinux.cfg/default"

	bootstrapScript_custom_privateSession="config/pxeConf.d/script_nonUforge/$bootstrapScript_fileName_privateSession" 
	bootstrapScript_system_privateSession="$bootDirPath/$bootstrapScript_fileName_privateSession"

	bootstrapScript_custom_privateSession_imageSpecific="config/pxeConf.d/script_nonUforge/$bootstrapScript_fileName_privateSession_imageSpecific"
	bootstrapScript_system_privateSession_imageSpecific="$bootDirPath/$bootstrapScript_fileName_privateSession_imageSpecific"

	mkdir "$bootDirPath/pxelinux.cfg"

	defaultLocalOsIso="$pathLocalOsIso/$defaultOs.iso"
	defaultLocalOsImage="$bootDirPath/$defaultOs"
	defaultPath_kernel="images/pxeboot"					# Path relative to the root path of the transfert server/$defaultOs
	defaultPath_fileSystem="images/pxeboot"					# Path relative to the root path of the transfert server/$defaultOs
#TODO +++++++ remove after change bootstrap.pxe (build uri in bootstrap)
	defaultUri_localRepo=$transfertProtocol://$nextServerIp:$nextServerPort/$defaultOs	# Path relative to the root path of the transfert server
										#	This URI is only used with pxe (only ftp transfert)
	mirorDir_toMount="/mnt/$defaultOs" 




#--------------------------------------
# REST URI
#--------------------------------------
	REST_URI_DEFAULT_IMAGE="ufws/users/root/appliances/4/images/3/downloads/ipxe/$defaultOs"
	REST_URI_PRIVATE_SESSION="ufws/users/root/appliances/4/images/3/downloads/ipxe/$bootstrapScript_fileName_privateSession" 
	REST_URI_PRIVATE_SESSION_IMAGE_SPECIFIC="ufws/users/root/appliances/4/images/3/downloads/ipxe/$bootstrapScript_fileName_privateSession_imageSpecific" 
	REST_URI_SPECIFIC_IMAGE="ufws/users/root/appliances/${aid}/images/\${iid}/downloads/ipxe/$defaultOs"


#--------------------------------------
# Mark services and data to be erased
#--------------------------------------
	echo $mirorDir_toMount			>> $tmpToEraseFileDir/env_localMirorMountPath
	echo $exec_dhcp				>> $tmpToEraseFileDir/env_serviceDhcp
	echo $exec_dns				>> $tmpToEraseFileDir/env_serviceDns
	echo $fileTransfertServiceList		>> $tmpToEraseFileDir/env_serviceListFileTransfert
	echo $transfertServerRootPathList	>> $tmpToEraseFileDir/env_transfertServerRootPathList

$exec_newLine
echo "--------------------------------------"
echo " Install and configure DNS service"
echo "--------------------------------------"
	# Install DNSMASQ deamon
	#TODO	yum install dnsmasq

	# Create a custom dnsmasq config file
	$exec_setDnsmasqConfiguration $dnsmasqConfigFile_system $networkInterface $nextServerIp $bootDirPath


$exec_newLine
echo "--------------------------------------"
echo " Install and configure DHCP Server"
echo "--------------------------------------"
	#TODO	yum install $exec_dhcp

	# Create the DHCP config files
	$exec_setDhcpConfiguration $dhcpConfigFile_system $dhcpConfigFile_custom $dhcpConfigFile_client_system $dhcpConfigFile_client_custom $ip $ip $nextServerIp $bootstrapBin_fileName $networkInterface $ip $subnetIp $subnetMask $addressRangeMin $addressRangeMax


$exec_newLine
echo "-------------------------------------"
echo " Install SYSLINUX Bootloaders"
echo "-------------------------------------"
	#TODO	yum install syslinux


$exec_newLine
echo "-------------------------------------"
echo " Install FTP-Server and Populate it with SYSLINUX Bootloaders"
echo "------------------------------------"
	#TODO	yum install ftp-server
	mkdir ${bootDirPath_base}


	if [ "$exec_extraTransfert" != "" ]; then 
$exec_newLine
echo "--------------------------------------"
echo "Install ${transfertProtocol}-Server ($exec_extraTransfert)"
echo "--------------------------------------"
	#TODO	yum install $exec_extraTransfert
	fi


$exec_newLine
echo "-------------------------------------"
echo " Populate ${transfertProtocol}-Server with SYSLINUX Bootloaders"
echo "------------------------------------"
	cp -r $syslinuxDir/* $bootDirPath


$exec_newLine
echo "------------------------------------"
echo " Setup $pxeProtocol Configuration File and bootstrap binary files"
echo "------------------------------------"
	cp ${bootstrapBin_custom_path}/${bootstrapBin_fileName} $bootDirPath_base
#	cp ${bootstrapBin_custom_path}/${bootstrapScript_fileName_privateSession} $bootDirPath
#	cp ${bootstrapBin_custom_path}/${bootstrapScript_fileName_privateSession_imageSpecific} $bootDirPath

	$exec_setBootstrapScript $bootstrapScript_system_mainPage $bootstrapScript_custom_mainPage $bootstrapScript_system_privateSession $bootstrapScript_custom_privateSession $bootstrapScript_system_privateSession_imageSpecific $bootstrapScript_custom_privateSession_imageSpecific $defaultUri_localRepo $defaultOs $defaultPath_kernel $defaultPath_fileSystem $defaultFileName_kernel $defaultFileName_fileSystem $transfertProtocol $nextServerIp $nextServerPort $REST_URI_DEFAULT_IMAGE $REST_URI_PRIVATE_SESSION $REST_URI_PRIVATE_SESSION_IMAGE_SPECIFIC $REST_URI_SPECIFIC_IMAGE

$exec_newLine
echo "------------------------------------"
echo " Add ISO Boot Image (mounted) to PXE Server"
echo "------------------------------------"
	# Instal the ISO image on server
	if [ $isoMount == "1" ]; then
		mkdir $mirorDir_toMount
		mount -o loop $defaultLocalOsIso $mirorDir_toMount
	fi


$exec_newLine
echo "--------------------------------------"
echo " Create $defaultOs Local Mirror Installation Source (using $transfertProtocol)"
echo "--------------------------------------"
#TODO	yum install $exec_ftp
	if [ $mirorInstall == "1" ]; then
		mkdir $defaultLocalOsImage
		cp -r -f $mirorDir_toMount/*  $defaultLocalOsImage/
		chmod -R 755 $defaultLocalOsImage/*
	fi




$exec_newLine
echo "-------------------------------------"
echo " Start and Enable Daemons System-Wide"
echo "-------------------------------------"
	service $exec_dhcp start
	service $exec_dhcp status

	service $exec_ftp start
	service $exec_ftp status

	service $exec_dns start
	service $exec_dns status

	if [ "$exec_extraTransfert" != "" ]; then 
		service $exec_extraTransfert start
		service $exec_extraTransfert status
	fi


$exec_newLine
echo "---------------------------------------"
echo " Open Firewall"
echo "---------------------------------------"
#	netstat -tulpn
#	firewall-cmd --add-service=ftp --permanent	## Port 21
#	firewall-cmd --add-service=dns --permanent	## Port 53
#	firewall-cmd --add-service=dhcp --permanent	## Port 67
#	firewall-cmd --add-port=69/udp --permanent	## Port for FTP
#	firewall-cmd --add-port=4011/udp --permanent	## Port for ProxyDHCP
#	firewall-cmd --reload				## Apply rules
