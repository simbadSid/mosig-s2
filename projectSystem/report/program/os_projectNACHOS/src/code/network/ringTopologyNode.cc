/*
 * ringTopologyTest.cc
 *
 *  Created on: Jan 23, 2016
 *      Author: littlegirle
 */

#include "copyright.h"
#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"


#define		MAX_NBR_RING_NODE	30
#define		DATA				"Hello there!"
#define		ACK					"Got it!"


void sendMsg(int destinationMachineId, const char *msg, int srcMailBox, int dstMailBox)
{
	PacketHeader outPktHdr;
	MailHeader outMailHdr;

	outPktHdr.to		= destinationMachineId;					// construct packet, mail header for original message To: destination machine, mailbox 0
	outMailHdr.to		= dstMailBox;							//		From: our machine, reply to: mailbox 1
	outMailHdr.from		= srcMailBox;
	outMailHdr.length	= strlen(msg) + 1;
	postOffice->Send(outPktHdr, outMailHdr, msg);				// Send the first message
	fflush(stdout);
}

void receiveMsg(int receiverMachineId, char *msg, int mailBox)
{
	PacketHeader inPktHdr;
	MailHeader inMailHdr;

	postOffice->Receive(mailBox, &inPktHdr, &inMailHdr, msg);	// Wait for the first message from the other machine
	printf("machineId = %d:\t Got \"%s\" from %d, box %d\n",
			receiverMachineId, msg, inPktHdr.from, inMailHdr.from);
	fflush(stdout);
}

void RingTopologyNode(unsigned int nbrRingNode)
{
	ASSERT(nbrRingNode	<=MAX_NBR_RING_NODE);
	ASSERT(nbrRingNode	> 1);

	char buffer[MaxMailSize];
	int currentMachineAddress	= postOffice->GetNeworkAdress();
	int forwardMachineAddress	= (currentMachineAddress+1) % nbrRingNode;

	if (currentMachineAddress == 0)
	{
		DEBUG('n', "Machine %d send to machine %d\n", currentMachineAddress, forwardMachineAddress);
//printf("Start send\n");
		sendMsg(forwardMachineAddress, DATA, 0, 1);
//printf("End send\n");
		receiveMsg(currentMachineAddress, buffer, 1);
	}
	else
	{
//printf("Start receive\n");
		receiveMsg(currentMachineAddress, buffer, 1);
//printf("End receive\n");
		sendMsg(forwardMachineAddress, buffer, 0, 1);
	}

	interrupt->Halt();
}
