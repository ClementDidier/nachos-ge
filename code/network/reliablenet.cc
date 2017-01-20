#include "reliablenet.h"

void Helper (int arg)
{
	ReliableNet* rn = (ReliableNet*) arg;
	rn->WaitMessages();
}

void ReadAvail(int arg)
{
	ReliableNet* rn = (ReliableNet*) arg;
	rn->IncomingPacket();
}

void WriteDone(int arg)
{
	ReliableNet* rn = (ReliableNet*) arg;
	rn->PacketSent();
}

ReliableNet::ReliableNet(NetworkAddress addr, double reliability, NetworkAddress target)
{
	messages = new SynchList();
	pktHdr.to = addr;

	this.target = target;

	memoryIndex = 0;
	memorySize = 0;

	numEmission = 0;
	numReception = 0;
	numAquitement = 0;

	messageAvailable = new Semaphore("message available", 0);
    messageSent = new Semaphore("message sent", 0);
    sendLock = new Lock("message send lock");

	network = new Network(addr, reliability, ReadAvail, WriteDone, (int) this);

	Thread *t = new Thread("postal worker");

    t->Fork(Helper, (int) this);
}

ReliableNet::~ReliableNet()
{
	delete messages;
	delete network;
	delete messageAvailable;
    delete messageSent;
    delete sendLock;
}

void ReliableNet::Send(const char *data)
{

}

int ReliableNet::Receive(char *data, int size)
{

}

void ReliableNet::PacketSent()
{
	messageSent->V();
}

void ReliableNet::IncomingPacket()
{
	messageAvailable->V();
}

void ReliableNet::WaitMessages()
{
	char *buffer = new char[MaxPacketSize];

    while(1)
    {
        // first, wait for a message
        messageAvailable->P();	
        pktHdr = network->Receive(buffer);

        Paquet p = new Paquet(pktHdr, buffer);
		messages->Append((void*) p);
    }
}

void ReliableNet::SetTarget (NetworkAddress addr)
{
	target = addr;
}


class Paquet
{
	PacketHeader hdr;
	char* data;

	Paquet::Paquet(PacketHeader header, char* data)
	{
		hdr = header;
		this.data = data;
	}
		
}