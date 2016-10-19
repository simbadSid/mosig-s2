#!/bin/sh

. $TESTDIR/common.sh 

tcptarget -p $PORT &
TPID=$!

waitsock

tcpmt -d 2 -p $PORT  -b 8192 localhost 
#check if the target exited as it should after transfer is complete    
sleep 1
! kill $TPID 2>/dev/null; rt=$?
if [ $rt -eq 1 ]
    then echo No connection was established with the target
    exit 1 
    else echo tcptarget terminated normally
fi


