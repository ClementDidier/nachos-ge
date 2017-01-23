#include "reliablenet.h"
#include <strings.h>

#define MSG 0x00
#define ACK 0x01
#define DATA_SIZE 100

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

	isClosed = 0;

	this.target = target;

	memoryIndex = 0;
	memorySize = 0;

	numEmission = 0;
	numReception = 0;
	numAquitement = 0;

	messageAvailable = new Semaphore("message available", 0);
    messageSent = new Semaphore("message sent", 0);
    sendLock = new Lock("message send lock");
    semAck = new Semaphore("message ack");

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

void ReliableNet::Send(char type, const char *data)
{
	switch(type)
	{
		case ACK:

			break;
		case MSG:
			break;
		default:
			break;
	}
	if(type == ACK)
	{

	}
	else if(type == MSG)
	semAck->P();
	//
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
        pktHdrReliable = ParseReliableHeader(buffer);


        switch(pktHdrReliable.type)
        {
        	case ACK:
        		if(pktHdrReliable.index == numEmission)
        		{
        			numAquitement = numEmission;
        			semAck->V();
        		}
        		else
        		{
        			printf("ReliableNet : ACK ignoré\n");
        		}
        		break;
        	case MSG:
        		if(numReception == pktHdrReliable.index - 1 || numReception == pktHdrReliable.index)
        		{
        			numReception = pktHdrReliable.index;
        			Send(ACK, "ACK_DATA\0");
        		}
        		else
        		{
        			printf("ReliableNet Erreur : Numéro du message reçu incorrect\n");
        		}
        		break;
        	case END:
        		//isClosed = 1;
        		break;
        	default:
        		printf("ReliableNet Erreur : Type de paquet inconnu !\n");
        		Exit(0);
        		break;
        }

        Paquet p = new Paquet(pktHdr, buffer);
		messages->Append((void*) p);
    }
}


PacketHeaderReliable ParseReliableHeader(char * buffer)
{
	PacketHeaderReliable pktHdr = *(PacketHeaderReliable*)buffer;

	char buff[DATA_SIZE];
	bcopy(buffer + sizeof(pktHdr), buff, DATA_SIZE);
	bcopy(buff, buffer, DATA_SIZE);

	return pktHdr;
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