#!/bin/sh

. $TESTDIR/common.sh 

OUTFILE=`mktemp -u /tmp/tcpmt.XXXX` || exit 1
INFILE=`mktemp -u /tmp/test-file.XXXX` || exit 1

echo "[0 1 222]{30 5 [20 2 444]}" > $INFILE

tcptarget -p $PORT -P > $OUTFILE   &
TPID=$!

waitsock

tcpmt -f $INFILE -p $PORT localhost 
#check if the target exited as it should after transfer is complete    
sleep 1

kill $TPID 
test `cat  $OUTFILE | grep "received" | awk '{printf("%d", $1)}'` -eq "222888888888888888"
rt=$?
rm $OUTFILE
rm $INFILE

if (( $rt != 0 )) ; 
    then exit 1;
fi
