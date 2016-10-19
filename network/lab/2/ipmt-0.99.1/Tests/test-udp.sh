#!/bin/sh
set -e 

OUTFILE=`mktemp -u /tmp/udpmt.XXXX` || exit 1

udptarget -o $OUTFILE -p 13001 &
TPID=$!
udpmt -d 2 -r 100 -p 13001 localhost 
kill $TPID
nb=`wc -c $OUTFILE | awk '{print $1}'`
if [ $nb -eq 0 ]
    then exit 1
    else echo "Udpmt actually received something"
fi
rm $OUTFILE