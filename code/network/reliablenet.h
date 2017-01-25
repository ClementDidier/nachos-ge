#ifndef RELIABLENET_H
#define RELIABLENET_H

#include "network.h"
#include "synchlist.h"

#define MSG 46
#define ACK 47
#define DATA_SIZE 30
#define MAXREEMISSIONS 10
#define TEMPO 2

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

		void Send(char type, const char *data, int size);
		int Receive(char *data, int size);

		void PacketSent();		// Interrupt handler, called when outgoing 
				// packet has been put on network; next 
				// packet can now be sent
    	void IncomingPacket();	// Interrupt handler, called when incoming
   				// packet has arrived and can be pulled
				// off of network (i.e., time to call 
				// PostalDelivery)
    	void WaitMessages();

    	int numEmission;
    	int numReception;
    	int numAquitement;
    	Semaphore * messageAvailable;// V'ed when message has arrived from network
    	Semaphore * messageSent;	// V'ed when next message can be sent to network
    	Semaphore * semAck;
    	Lock *sendLock;		// Only one outgoing message at a time
    	Network* network;

	private:
		PacketHeaderReliable * ParseReliableHeader(char * buffer);
		
		SynchList * messages;
		
		NetworkAddress from;
		NetworkAddress target;

    	char memory[MaxPacketSize];
    	int memoryIndex;
    	int memorySize;

    	

    	
};

typedef struct
{
	PacketHeader hdr;
	PacketHeaderReliable hdrReliable;
	char* data;
	ReliableNet * rlbnet;
} PacketContext;

#endif

