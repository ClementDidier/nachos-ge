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

void printRelHdr(PacketHeaderReliable hdr)
{
	printf("------------- ENTETE RELIABLE NET -----------\nType : %d\nIndex : %d\n---------------------------------------------\n", hdr.type, hdr.index);
}

PacketHeaderReliable ReliableNet::ParseReliableHeader(char * buffer)
{
	PacketHeaderReliable pktHdrRel = *(PacketHeaderReliable*)buffer;

	printRelHdr(pktHdrRel);

	char buff[DATA_SIZE];
	bcopy(buffer + sizeof(PacketHeaderReliable), buff, DATA_SIZE);
	bcopy(buff, buffer, DATA_SIZE);

	return pktHdrRel;
}

ReliableNet::ReliableNet(NetworkAddress addr, double reliability, NetworkAddress to)
{
	messages = new SynchList();
	pktHdr.to = to;
	pktHdr.from = addr;
	pktHdr.length = DATA_SIZE; // Data de taille statique

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
	printf("Début ThreadHandler Send\n");

	PacketContext * context = (PacketContext *) arg;

	char * buffer = new char[DATA_SIZE + sizeof(PacketHeaderReliable)];

	printRelHdr(context->hdrReliable);
	//bcopy((void*)&context->hdrReliable.type, buffer, sizeof(char));
	//bcopy((void*)&context->hdrReliable.index, buffer + sizeof(char), sizeof(int));
	//*(PacketHeaderReliable *) buffer = context->hdrReliable;
	*(PacketHeaderReliable *)buffer = context->hdrReliable;

	printf("Buffer : <%s> & Buffer Data : %s\n", buffer, context->data);

	bcopy(context->data, buffer + sizeof(PacketHeaderReliable), context->hdr.length);


	int i;
	for(i = 0; i < MAXREEMISSIONS; i++)
	{
		if(context->rlbnet->numEmission == context->rlbnet->numAquitement && context->rlbnet->numEmission > 0) // Message acquité
		{
			break;
		}

		printf("Acquire in send *****\n");
		context->rlbnet->sendLock->Acquire();

		printf("network send in send *****\n");
    	context->rlbnet->network->Send(context->hdr, context->data);
    	context->rlbnet->messageSent->P();

    	context->rlbnet->sendLock->Acquire();
    	
		Delay(TEMPO);	
	}

	delete [] buffer;
}

void ReliableNet::Send(char type, const char *data, int size)
{
	switch(type)
	{
		case ACK:
			network->Send(pktHdr, (char*)data);
			break;
		case MSG:
			printf("MSG before Semaphore\n");
			semAck->P();
			printf("MSG after Semaphore\n");

			PacketContext context;
			context.hdr = pktHdr;
			context.hdr.length = size;
			context.hdrReliable.type = type;
			context.hdrReliable.index = numEmission;
			context.data = (char*)data;
			/*context.nE = numEmission;
			context.nA = numAquitement;
			context.network = network;*/
			context.rlbnet = this;

			Thread * t = new Thread("send worker");
			t->Fork(ThreadSendHandler, (int) &context);
			currentThread->Yield();
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

	printf("receive realised : messages->IsEmpty = %d\n", messages->IsEmpty());
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

        			Send(ACK, "A", 1);
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