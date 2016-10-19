// post.h 
//	Data structures for providing the abstraction of unreliable,
//	ordered, fixed-size message delivery to mailboxes on other 
//	(directly connected) machines.  Messages can be dropped by
//	the network, but they are never corrupted.
//
// 	The US Post Office delivers mail to the addressed mailbox. 
// 	By analogy, our post office delivers packets to a specific buffer 
// 	(MailBox), based on the mailbox number stored in the packet header.
// 	Mail waits in the box until a thread asks for it; if the mailbox
//      is empty, threads can wait for mail to arrive in it. 
//
// 	Thus, the service our post office provides is to de-multiplex 
// 	incoming packets, delivering them to the appropriate thread.
//
//      With each message, you get a return address, which consists of a "from
// 	address", which is the id of the machine that sent the message, and
// 	a "from box", which is the number of a mailbox on the sending machine 
//	to which you can send an acknowledgement, if your protocol requires 
//	this.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef POST_H
#define POST_H

#include "network.h"
#include "synchlist.h"
#include "timer.h"
// +b simbadSid 26.01.2016
//#include "system.h"
// +e simbadSid 26.01.2016



#define MAX_NBR_MSG_SEND	20											// Number of tries to send a msg
#define RESEND_WAITING_TIME	200											// Number of scheduler click before resent a msg waiting for ack


typedef int MailBoxAddress;												// Mailbox address -- uniquely identifies a mailbox on a given machine.
																		// A mailbox is just a place for temporary storage for messages.
// ---------------------------------------------
// Type of an ack in a mail msg
// ---------------------------------------------
typedef unsigned int ack_t;


// -----------------------------------------------------------
// The following class defines part of the message header.  
// This is prepended to the message by the PostOffice, before the message 
// is sent to the Network.
// -----------------------------------------------------------
class MailHeader
{
  public:
    MailBoxAddress	to;													// Destination mail box
    MailBoxAddress	from;												// Mail box to reply to
    // +b simbadSid 25.01.2015
    bool			isAck;
    ack_t				ackId;												// Id of an acknowledgment (0 if it the mail is not an ack)
    // +e simbadSid 25.01.2015
    unsigned		length;												// Bytes of message data (excluding the mail header)
};

// -----------------------------------------------------------
// Maximum "payload" -- real data -- that can included in a single message
// Excluding the MailHeader and the PacketHeader
// -----------------------------------------------------------
#define MaxMailSize 	(MaxPacketSize - sizeof(MailHeader))


// -----------------------------------------------------------
// The following class defines the format of an incoming/outgoing 
// "Mail" message.  The message format is layered: 
//	network header (PacketHeader) 
//	post office header (MailHeader) data
// -----------------------------------------------------------
class Mail
{
  public:
     Mail(PacketHeader pktH, MailHeader mailH, char *msgData);			// Initialize a mail message by concatenating the headers to the data
     PacketHeader	pktHdr;												// Header appended by Network
     MailHeader		mailHdr;											// Header appended by PostOffice
     char			data[MaxMailSize];									// Payload -- message data
};

// -----------------------------------------------------------
// The following class defines a single mailbox, or temporary storage
// for messages.   Incoming messages are put by the PostOffice into the 
// appropriate mailbox, and these messages can then be retrieved by
// threads on this machine.
// -----------------------------------------------------------
class MailBox
{
  public: 
    MailBox();															// Allocate and initialize mail box
    ~MailBox();															// De-allocate mail box
    void Put(PacketHeader pktHdr, MailHeader mailHdr, char *data);		// Atomically put a message into the mailbox
    void Get(PacketHeader *pktHdr, MailHeader *mailHdr, char *data);	// Atomically get a message out of the
    																	// mailbox (and wait if there is no message to get!)
  private:
    SynchList	*messages;												// A mailbox is just a list of arrived messages
};

// -----------------------------------------------------------
// The following class defines a "Post Office", or a collection of 
// mailboxes.  The Post Office is a synchronization object that provides
// two main operations:
//		- Send -- send a message to a mailbox on a remote machine.
//		- Receive -- wait until a message is in the mailbox, then remove and return it.
// Incoming messages are put by the PostOffice into the 
// appropriate mailbox, waking up any threads waiting on Receive.
// -----------------------------------------------------------
class PostOffice
{
  public:
	PostOffice(NetworkAddress addr, double reliability, int nBoxes);	// Allocate and initialize Post Office "reliability" is how many packets
																		//   get dropped by the underlying network
	~PostOffice();														// De-allocate Post Office data

	bool Send(PacketHeader pktHdr,MailHeader mailHdr, const char *data);// Send a message to a mailbox on a remote  machine.
																		// 	The fromBox in the MailHeader is the return box for ack's.
	void Receive(int box, PacketHeader *pktHdr, MailHeader *mailHdr,	// Retrieve a message from "box".  Wait if
			char *data);												//	there is no message in the box.
	void PostalDelivery();												// Wait for incoming messages, and then put them in the correct mailbox
	//+b simbadSid 25.01.2016
	void CheckAck();													// Handler to the timer: Wake up all the threads waiting for an acknowledgment
	//+e simbadSid 25.01.2016
	void PacketSent();													// Interrupt handler, called when outgoing packet has been put on network; next
																		//	packet can now be sent
	void IncomingPacket();												// Interrupt handler, called when incoming packet has arrived and can be pulled
																		// off of network (i.e., time to call PostalDelivery)
	NetworkAddress GetNeworkAdress(){return this->netAddr;}

  private:
	// Private mathodes
//+b simbadSid 25.01.2016
	bool Send(PacketHeader pktHdr,MailHeader mailHdr, const char *data, bool IS_ACK, ack_t ACK_ID);
	void SendSimple(PacketHeader pktHdr, char *msg);					// Send a single msg and don't wait for ack
	bool SendUntilACK(PacketHeader pktHdr, char *msg, ack_t ack);		// Send several msg and wait for 1 ack
	int	GenerateACK();
//+e simbadSid 25.01.2016

	// Private attributes
	Network			*network;											// Physical network connection
	NetworkAddress	netAddr;											// Network address of this machine
	MailBox			*boxes;												// Table of mail boxes to hold incoming mail
	int				numBoxes;											// Number of mail boxes
	Semaphore		*messageAvailable;									// V'ed when message has arrived from network
	Semaphore		*messageSent;										// V'ed when next message can be sent to network
	Lock			*sendLock;											// Only one outgoing message at a time
//+b simbadSid 25.01.2016
	KeyList			*pendingSentMsg;									// List of msg waiting for acknowledgment
	Lock			*pendingSentLock;									// Lock to access the pendingSentMsg list
//+e simbadSid 25.01.2016
};


class PendingSentMsg
{
	public:
		// Attributes
		int			nbrTry;
		Condition	*cond;
		Lock		*lock;
		bool		delivered;

		// Builder
		PendingSentMsg(Lock *LOCK)
		{
			this->nbrTry	= 0;
			this->cond		= new Condition("Condition var for pending msg");
			this->lock		= LOCK;
			this->delivered	= false;
		}
		~PendingSentMsg()
		{
			delete this->cond;
		}
};


#endif
