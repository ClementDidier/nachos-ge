#ifndef RELIABLENET_H
#define RELIABLENET_H

#include "post.h"

typedef struct 
{
	char type;
	int index;
} PacketHeaderReliable;

class ReliableNet
{
	public:
		ReliableNet(NetworkAddress addr, double reliability, NetworkAddress target);
		~ReliableNet();

		void Send(const char *data);
		int Receive(char *data, int size);

		void PacketSent();		// Interrupt handler, called when outgoing 
				// packet has been put on network; next 
				// packet can now be sent
    	void IncomingPacket();	// Interrupt handler, called when incoming
   				// packet has arrived and can be pulled
				// off of network (i.e., time to call 
				// PostalDelivery)
    	void WaitMessages();

	private:

		PacketHeaderReliable ParseReliableHeader(char * buffer);

		Network* net;
		PacketHeader pktHdr;
		PacketHeaderReliable pktHdrReliable;
		SynchList* messages;
		Semaphore *messageAvailable;// V'ed when message has arrived from network
    	Semaphore *messageSent;	// V'ed when next message can be sent to network
    	Lock *sendLock;		// Only one outgoing message at a time
    	NetworkAddress target;
    	char memory[MaxPacketSize];
    	int memoryIndex;
    	int memorySize;

    	int isClosed;

    	int numEmission;
    	int numReception;
    	int numAquitement;
}

#endif

