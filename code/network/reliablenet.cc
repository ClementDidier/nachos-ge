#include "reliablenet.h"
#include <strings.h>
#include "system.h"

class Paquet
{
	public:
		PacketHeader hdr;
		char* data;

		Paquet(PacketHeader header, char* buffer)
		{
			hdr = header;
			data = buffer;
		}
};

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

PacketHeaderReliable ReliableNet::ParseReliableHeader(char * buffer)
{
	PacketHeaderReliable pktHdrRel = *(PacketHeaderReliable*)buffer;

	char buff[DATA_SIZE];
	bcopy(buffer + sizeof(pktHdrRel), buff, DATA_SIZE);
	bcopy(buff, buffer, DATA_SIZE);

	return pktHdrRel;
}

ReliableNet::ReliableNet(NetworkAddress addr, double reliability, NetworkAddress to)
{
	messages = new SynchList();
	pktHdr.to = addr;

	target = to;

	memoryIndex = 0;
	memorySize = 0;

	numEmission = 0;
	numReception = 0;
	numAquitement = 0;

	messageAvailable = new Semaphore("message available", 0);
    messageSent = new Semaphore("message sent", 0);
    sendLock = new Lock("message send lock");
    semAck = new Semaphore("message ack", 1);

	network = new Network(addr, reliability, ReadAvail, WriteDone, (int) this);

	Thread *t = new Thread("postal worker");

    t->Fork(Helper, (int) this);
    currentThread->Yield();
}

ReliableNet::~ReliableNet()
{
	delete messages;
	delete network;
	delete messageAvailable;
    delete messageSent;
    delete sendLock;
}

void ThreadSendHandler(int arg)
{
	PacketContext * context = (PacketContext *) arg;

	char * buffer = new char[DATA_SIZE + sizeof(PacketHeaderReliable)];
	*(PacketHeaderReliable *) buffer = context->hdrReliable;
	bcopy(context->data, buffer, sizeof(PacketHeaderReliable));

	int i;
	for(i = 0; i < MAXREEMISSIONS; i++)
	{
		if(context->rlbnet->numEmission == context->rlbnet->numAquitement) // Message acquité
		{
			break;
		}

		context->rlbnet->sendLock->Acquire();

    	context->rlbnet->network->Send(context->hdr, context->data);
    	context->rlbnet->messageSent->P();

    	context->rlbnet->sendLock->Acquire();
    	
		Delay(TEMPO);	
	}

	delete [] buffer;
}

void ReliableNet::Send(char type, const char *data)
{
	switch(type)
	{
		case ACK:
			network->Send(pktHdr, (char*)data);
			break;
		case MSG:
			semAck->P();
			//ASSERT(numEmission != numAquitement);

			PacketContext context;
			context.hdr = pktHdr;
			context.hdrReliable.type = type;
			context.hdrReliable.index = numEmission;
			context.data = (char*)data;
			/*context.nE = numEmission;
			context.nA = numAquitement;
			context.network = network;*/
			context.rlbnet = this;

			Thread * t = new Thread("send worker");
			t->Fork(ThreadSendHandler, (int) &context);
			//semAck->P();
			break;
	}
}

int ReliableNet::Receive(char *data, int size)
{
	int s = size / DATA_SIZE;
	int i;
	for(i = 0; i < s && !messages->IsEmpty(); i++)
	{
		Paquet * paquet = (Paquet*) messages->Remove();
		bcopy(paquet->data, data + i * DATA_SIZE, DATA_SIZE);
	}

	if(messages->IsEmpty())
		return 0;
	return 1;
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
	printf("Waiting...\n");
	char *buffer = new char[MaxPacketSize];

    while(1)
    {
        // first, wait for a message
        messageAvailable->P();
        pktHdr = network->Receive(buffer);
        pktHdrReliable = ParseReliableHeader(buffer);

        printf("ReliableNet : Message reçu [%s] <%s>\n", pktHdrReliable.type == ACK ? "ACK" : (pktHdrReliable.type == MSG ? "MSG" : "ERROR"), buffer);


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
        			if(numReception != pktHdrReliable.index)
        			{
	        			numReception = pktHdrReliable.index;

	        			Paquet * p = new Paquet(pktHdr, buffer);
						messages->Append((void*) p);
					}

        			Send(ACK, "ACK_DATA\0");
        		}
        		else
        		{
        			printf("ReliableNet Erreur : Numéro du message reçu incorrect\n");
        		}
        		break;
        	default:
        		printf("ReliableNet Erreur : Type de paquet inconnu !\n");
        		Exit(0);
        		break;
        }
    }
}