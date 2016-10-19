// post.cc 
// 	Routines to deliver incoming network messages to the correct
//	"address" -- a mailbox, or a holding area for incoming messages.
//	This module operates just like the US postal service (in other
//	words, it works, but it's slow, and you can't really be sure if
//	your mail really got through!).
//
//	Note that once we prepend the MailHdr to the outgoing message data,
//	the combination (MailHdr plus data) looks like "data" to the Network 
//	device.
//
// 	The implementation synchronizes incoming messages with threads
//	waiting for those messages.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "post.h"

#include <strings.h> /* for bzero */



//----------------------------------------------------------------------
// Mail::Mail
//      Initialize a single mail message, by concatenating the headers to
//	the data.
//
//	"pktH" -- source, destination machine ID's
//	"mailH" -- source, destination mailbox ID's
//	"data" -- payload data
//----------------------------------------------------------------------

Mail::Mail(PacketHeader pktH, MailHeader mailH, char *msgData)
{
    ASSERT(mailH.length <= MaxMailSize);

    pktHdr = pktH;
    mailHdr = mailH;
    bcopy(msgData, data, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBox::MailBox
//      Initialize a single mail box within the post office, so that it
//	can receive incoming messages.
//
//	Just initialize a list of messages, representing the mailbox.
//----------------------------------------------------------------------


MailBox::MailBox()
{ 
    messages = new SynchList();
}

//----------------------------------------------------------------------
// MailBox::~MailBox
//      De-allocate a single mail box within the post office.
//
//	Just delete the mailbox, and throw away all the queued messages 
//	in the mailbox.
//----------------------------------------------------------------------

MailBox::~MailBox()
{ 
    delete messages; 
}

//----------------------------------------------------------------------
// PrintHeader
// 	Print the message header -- the destination machine ID and mailbox
//	#, source machine ID and mailbox #, and message length.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//----------------------------------------------------------------------

static void 
PrintHeader(PacketHeader pktHdr, MailHeader mailHdr)
{
    printf("From (%d, %d) to (%d, %d) bytes %d\n",
    	    pktHdr.from, mailHdr.from, pktHdr.to, mailHdr.to, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBox::Put
// 	Add a message to the mailbox.  If anyone is waiting for message
//	arrival, wake them up!
//
//	We need to reconstruct the Mail message (by concatenating the headers
//	to the data), to simplify queueing the message on the SynchList.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
//----------------------------------------------------------------------

void 
MailBox::Put(PacketHeader pktHdr, MailHeader mailHdr, char *data)
{ 
    Mail *mail = new Mail(pktHdr, mailHdr, data); 

    messages->Append((void *)mail);	// put on the end of the list of 
									// arrived messages, and wake up
									// any waiters
}

//----------------------------------------------------------------------
// MailBox::Get
// 	Get a message from a mailbox, parsing it into the packet header,
//	mailbox header, and data. 
//
//	The calling thread waits if there are no messages in the mailbox.
//
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------

void 
MailBox::Get(PacketHeader *pktHdr, MailHeader *mailHdr, char *data) 
{ 
    DEBUG('n', "Waiting for mail in mailbox\n");
    Mail *mail = (Mail *) messages->Remove();			// remove message from list. Will wait if list is empty

    *pktHdr = mail->pktHdr;
    *mailHdr = mail->mailHdr;
    if (DebugIsEnabled('n'))
    {
		printf("Got mail from mailbox: ");
		PrintHeader(*pktHdr, *mailHdr);
    }
    bcopy(mail->data, data, mail->mailHdr.length);		// copy the message data into the caller's buffer
    delete mail;										// we've copied out the stuff we need, we can now discard the message
}

//----------------------------------------------------------------------
// PostalHelper, ReadAvail, WriteDone
// 	Dummy functions because C++ can't indirectly invoke member functions
//	The first is forked as part of the "postal worker thread; the
//	later two are called by the network interrupt handler.
//
//	"arg" -- pointer to the Post Office managing the Network
//----------------------------------------------------------------------
static void PostalHelper(int arg)	{ PostOffice* po = (PostOffice *) arg; po->PostalDelivery(); }
//static void AckHelper(int arg)		{ PostOffice* po = (PostOffice *) arg; po->CheckAck(); }
static void ReadAvail(int arg)		{ PostOffice* po = (PostOffice *) arg; po->IncomingPacket(); }
static void WriteDone(int arg)		{ PostOffice* po = (PostOffice *) arg; po->PacketSent(); }

//----------------------------------------------------------------------
// PostOffice::PostOffice
// 	Initialize a post office as a collection of mailboxes.
//	Also initialize the network device, to allow post offices
//	on different machines to deliver messages to one another.
//
//      We use a separate thread "the postal worker" to wait for messages 
//	to arrive, and deliver them to the correct mailbox.  Note that
//	delivering messages to the mailboxes can't be done directly
//	by the interrupt handlers, because it requires a Lock.
//
//	"addr" is this machine's network ID 
//	"reliability" is the probability that a network packet will
//	  be delivered (e.g., reliability = 1 means the network never
//	  drops any packets; reliability = 0 means the network never
//	  delivers any packets)
//	"nBoxes" is the number of mail boxes in this Post Office
//----------------------------------------------------------------------
PostOffice::PostOffice(NetworkAddress addr, double reliability, int nBoxes)
{
	messageAvailable	= new Semaphore("message available", 0);		// Initialize the synchronization with the interrupt handlers
	messageSent			= new Semaphore("message sent", 0);
	sendLock			= new Lock("message send lock");

	netAddr				= addr;											// Initialize the mailboxes
	numBoxes			= nBoxes;
	boxes				= new MailBox[nBoxes];

	network				= new Network(addr, reliability, ReadAvail,		// Initialize the network; tell it which interrupt handlers to call
							WriteDone, (int) this);
	//+b simbadSid 25.01.2016
	this->pendingSentMsg	= new KeyList();							// Initialize the list of sent msg waiting for ack + lock for accessing the list
	this->pendingSentLock	= new Lock("Pending sent msg list lock");


	//+b simbadSid 15.01.2016
	// TODO Change the second parameter: 144
	Thread *t0 = new Thread("postal worker", 144);						// Create a thread whose sole job is to wait for incoming messages, and put them in the right mailbox.
	t0->Fork(PostalHelper, (int) this);

//	Thread *t1 = new Thread("postal worker", 145);						// Create a thread whose sole job is to wait for incoming messages, and put them in the right mailbox.
//	t1->Fork(AckHelper, (int) this);
	//+e simbadSid 15.01.2016

	//+e simbadSid 25.01.2016
}

//----------------------------------------------------------------------
// PostOffice::~PostOffice
// 	De-allocate the post office data structures.
//----------------------------------------------------------------------

PostOffice::~PostOffice()
{
    delete network;
    delete [] boxes;
    delete messageAvailable;
    delete messageSent;
    delete sendLock;
    // +b simbadSid 25.01.2016
    this->pendingSentMsg->FreeAllList();
    delete this->pendingSentMsg;
    delete this->pendingSentLock;
    // +e simbadSid 25.01.2016
}

//----------------------------------------------------------------------
// PostOffice::PostalDelivery
// 	Wait for incoming messages, and put them in the right mailbox.
//  If the incoming msg is
//		- An ack: wake up the waiting thread
//		- A mail: put it in the mail box
//      Incoming messages have had the PacketHeader stripped off,
//	but the MailHeader is still tacked on the front of the data.
//----------------------------------------------------------------------
void PostOffice::PostalDelivery()
{
    PacketHeader pktHdr;
    MailHeader mailHdr;
    char *buffer = new char[MaxPacketSize];

    for (;;)
    {
		// first, wait for a message
		messageAvailable->P();
		pktHdr = network->Receive(buffer);

		mailHdr = *(MailHeader *)buffer;

		if (DebugIsEnabled('n'))
		{
			const char *mailType = ((mailHdr.isAck) ? "ACK" : "User mail");
			printf("Putting mail into mailbox: (%s)", mailType);
			PrintHeader(pktHdr, mailHdr);
		}
		// check that arriving message is legal!
		ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
		ASSERT(mailHdr.length <= MaxMailSize);
//printf("Receive msg: (%s)\n", ((mailHdr.isAck)?"ACK":"User mail"));
        //+b simbadSid 25.01.2015
        if (!mailHdr.isAck)															// Case: mail is a msg
        {
//TODO
			PacketHeader ackPktHdr;													//		construct response containing the ack
			MailHeader ackMailHdr;
			ackPktHdr.to		= pktHdr.from;
			ackMailHdr.to		= mailHdr.from;
			ackMailHdr.from		= mailHdr.to;
			ackMailHdr.length	= 1;
			this->Send(ackPktHdr, ackMailHdr, "", true, mailHdr.ackId);				//		Send an ack to the sender

    		boxes[mailHdr.to].Put(pktHdr, mailHdr, buffer + sizeof(MailHeader));	//		Put the mail into mailbox
        }
        else																		// Case mail is an acknowledgment
        {
//TODO
			this->pendingSentLock->Acquire();										//		Begin critical section
			PendingSentMsg *psm;
			bool test = this->pendingSentMsg->IsInList(mailHdr.ackId, (void**)&psm);//		Look for the sent msg waiting for the arrived ack
			ASSERT(test);
			psm->delivered = true;
			psm->cond->Signal(this->pendingSentLock);								//		Notify the waiting thread that the ack arrived

			this->pendingSentLock->Release();										//		End critical section
        }
        //+e simbadSid 25.01.2015
    }
}

//+b simbadSid 25.01.2016
// TODO


static void wakePendingMsg(PendingSentMsg *psm)
{
	psm->cond->Signal(psm->lock);
}

//----------------------------------------------------------------------
// Handler to the timer: Wake up all the threads waiting for an acknowledgment
//----------------------------------------------------------------------
void PostOffice::CheckAck()
{
// TODO
	for (;;)
	{
		this->pendingSentLock->Acquire();									// Begin critical section
		if (!this->pendingSentMsg->IsEmpty())
		{
			this->pendingSentMsg->Mapcar((VoidFunctionPtr)wakePendingMsg);	// Wake up all the threads waiting for an ack
		}
		this->pendingSentLock->Release();									// End critical section
	}
}
//+e simbadSid 25.01.2016

//----------------------------------------------------------------------
// PostOffice::Send
// 	Concatenate the MailHeader to the front of the data, and pass 
//	the result to the Network for delivery to the destination machine.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
// Return true if the msg has been delivered and the ack received.
//----------------------------------------------------------------------
bool PostOffice::Send(PacketHeader pktHdr, MailHeader mailHdr, const char* data)
{
	return this->Send(pktHdr, mailHdr, data, false, GenerateACK());
}

bool PostOffice::Send(PacketHeader pktHdr, MailHeader mailHdr, const char* data, bool IS_ACK, ack_t ACK_ID)
{
	char* buffer = new char[MaxPacketSize];										// space to hold concatenated mailHdr + data
	bool res = true;

	if (DebugIsEnabled('n'))
	{
		printf("Post send: ");
		PrintHeader(pktHdr, mailHdr);
	}
	ASSERT(mailHdr.length <= MaxMailSize);
	ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);

	pktHdr.from		= netAddr;													// fill in pktHdr, for the Network layer
	pktHdr.length	= mailHdr.length + sizeof(MailHeader);
	// +b simbadSid 25.01.2016
//TODO
	mailHdr.isAck	= IS_ACK;
	mailHdr.ackId	= ACK_ID;

	bcopy(&mailHdr, buffer, sizeof(MailHeader));								// concatenate MailHeader and data
	bcopy(data, buffer + sizeof(MailHeader), mailHdr.length);

	if (IS_ACK)		this->SendSimple(pktHdr, buffer);							// Case ACK: send 1 msg without waiting for ack
	else			res = this->SendUntilACK(pktHdr, buffer, mailHdr.ackId);	// Case msg: send sevral msg and wait for 1 ack
	// +e simbadSid 25.01.2016

	delete [] buffer;															// we've sent the message, so we can delete our buffer
	if (!res)
	{
		DEBUG('n', "**** The destination has not returned any ACK: The msg may have been lost ****\n");
	}
	return res;
}

//----------------------------------------------------------------------
// PostOffice::Receive
// 	Retrieve a message from a specific box if one is available, 
//	otherwise wait for a message to arrive in the box.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//
//	"box" -- mailbox ID in which to look for message
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------
void PostOffice::Receive(int box, PacketHeader *pktHdr, MailHeader *mailHdr, char* data)
{
	ASSERT((box >= 0) && (box < numBoxes));

	boxes[box].Get(pktHdr, mailHdr, data);
	ASSERT(mailHdr->length <= MaxMailSize);
	// +b simbadSid 25.01.2016
	ASSERT(!mailHdr->isAck);										// Check that the mail is not an ack
	// +e simbadSid 25.01.2016
}

//----------------------------------------------------------------------
// PostOffice::IncomingPacket
// 	Interrupt handler, called when a packet arrives from the network.
//
//	Signal the PostalDelivery routine that it is time to get to work!
//----------------------------------------------------------------------
void PostOffice::IncomingPacket()
{ 
    messageAvailable->V(); 
}

//----------------------------------------------------------------------
// PostOffice::PacketSent
// 	Interrupt handler, called when the next packet can be put onto the 
//	network.
//
//	The name of this routine is a misnomer; if "reliability < 1",
//	the packet could have been dropped by the network, so it won't get
//	through.
//----------------------------------------------------------------------
void 
PostOffice::PacketSent()
{ 
    messageSent->V();
}


//+b simbadSid 25.01.2016
// TODO generate a unique ACK
static int toRemove = 0;
int PostOffice::GenerateACK()
{
	toRemove ++;
	return ((int)this + toRemove);
}

// ----------------------------------------------------------------------
// Send one msg and does not wait for ack
// ----------------------------------------------------------------------
void PostOffice::SendSimple(PacketHeader pktHdr, char *msg)
{
	sendLock->Acquire();										// only one message can be sent to the network at any one time
	network->Send(pktHdr, msg);
	messageSent->P();											// wait for interrupt to tell us ok to send the next message
	sendLock->Release();
}

// ----------------------------------------------------------------------
// Send several msg and wait for 1 ack.
// Return true if the msg has been delivered and the ack received.
// ----------------------------------------------------------------------
bool PostOffice::SendUntilACK(PacketHeader pktHdr, char *msg, ack_t ack)
{
//TODO
	bool res = false, test;

	this->pendingSentLock->Acquire();							// Begin critical section

	PendingSentMsg *psm = new PendingSentMsg(this->pendingSentLock), *psm2 = NULL;
	this->pendingSentMsg->Prepend(ack, (void*)psm);				// Put the pending mail in the list
	while (psm->nbrTry < MAX_NBR_MSG_SEND)						// While the number of mail sent is < security
//while(true)
	{
		psm->nbrTry ++;
		this->SendSimple(pktHdr, msg);							//		Send the mail
		psm->cond->Wait(this->pendingSentLock);					//		Wait for the ack response or for the timer
		test = this->pendingSentMsg->IsInList(ack, (void**)&psm2);
		ASSERT(test);
		ASSERT(psm == psm2);
		if (!psm->delivered) continue;
		res = true;
		break;
	}
	this->pendingSentMsg->Remove(ack, (void**)&psm2);
	ASSERT(psm == psm2);
	delete psm;

	this->pendingSentLock->Release();							// End critical section

	return res;
}
//+e simbadSid 25.01.2016
