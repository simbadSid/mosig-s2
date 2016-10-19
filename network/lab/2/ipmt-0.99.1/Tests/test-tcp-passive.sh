#!/bin/sh
. $TESTDIR/common.sh 

tcpmt -P -d 2 -p $PORT &
TPID=$!

waitsock

echo "Connecting target to source. This should not take long..."
tcptarget -p $PORT -A localhost 

#check if the source exited as it should after transfer is complete    
sleep 1
! kill $TPID  2>/dev/null; rt=$?
if [ $rt -eq 1 ]
    then echo "No connection was established with the source"
    exit 1 
    else echo "tcpmt terminated normally"
fi
