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

PacketHeaderReliable * ReliableNet::ParseReliableHeader(char * buffer)
{
	PacketHeaderReliable * pkt = new PacketHeaderReliable();
	memcpy(&pkt->type, buffer, sizeof(char));
	memcpy(&pkt->index, buffer + sizeof(char), sizeof(int));

	//printRelHdr(*pkt);

	char buff[DATA_SIZE];
	bcopy(buffer + sizeof(PacketHeaderReliable), buff, DATA_SIZE);
	bcopy(buff, buffer, DATA_SIZE);

	return pkt;
}

ReliableNet::ReliableNet(NetworkAddress addr, double reliability, NetworkAddress to)
{
	messages = new SynchList();
	from = addr;
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
	//printf("Début ThreadHandler Send\n");

	PacketContext * context = (PacketContext *) arg;

	char * buffer = new char[DATA_SIZE + sizeof(PacketHeaderReliable)];

	//printRelHdr(context->hdrReliable);

	memcpy(buffer, &context->hdrReliable.type, sizeof(char));
	memcpy(buffer + sizeof(char), &context->hdrReliable.index, sizeof(int));
	memcpy(buffer + sizeof(PacketHeaderReliable), context->data, context->hdr.length);

	context->hdr.length += sizeof(PacketHeaderReliable);

	//printf("\n\tBuffer : <%s> & Buffer Data : %s\n", buffer, context->data);


	int i;
	for(i = 0; i < MAXREEMISSIONS; i++)
	{
		if(context->hdrReliable.type == ACK && context->rlbnet->numAquitement == context->rlbnet->numReception - 1 && context->rlbnet->numAquitement > 0)
			break;
		if(context->hdrReliable.type == MSG && context->rlbnet->numEmission == context->rlbnet->numAquitement && context->rlbnet->numEmission > 0) // Message acquité
		{
			printf("Message acquité...\n");
			break;
		}
		context->rlbnet->sendLock->Acquire();
    	context->rlbnet->network->Send(context->hdr, buffer);

    	printf("Envoi d'un message [%s] #%d [ne : %d, na : %d, nr : %d]\n", 
    		context->hdrReliable.type == MSG ? "MSG" : "ACK", context->rlbnet->numEmission, 
    		context->rlbnet->numEmission,
    		context->rlbnet->numAquitement,
    		context->rlbnet->numReception);

    	context->rlbnet->messageSent->P();

    	context->rlbnet->sendLock->Release();
    	
    	currentThread->Yield();
		Delay(TEMPO);	
	}

	delete context;
	delete [] buffer;

	if(i == MAXREEMISSIONS)
		Exit(0);
}

void ReliableNet::Send(char type, const char *data, int size)
{
	switch((int)type)
	{
		case ACK:
		{
			PacketContext * contextACK = new PacketContext();
			contextACK->hdr.to = target;
			contextACK->hdr.from = from;
			contextACK->hdr.length = size;
			contextACK->hdrReliable.type = type;
			contextACK->hdrReliable.index = numReception;
			contextACK->data = (char*)data;
			contextACK->rlbnet = this;

			Thread * t1 = new Thread("send worker");
			t1->Fork(ThreadSendHandler, (int) contextACK);
			currentThread->Yield();

			break;
		}
		case MSG:
		{
			semAck->P();

			PacketContext * context = new PacketContext();
			context->hdr.to = target;
			context->hdr.from = from;
			context->hdr.length = size;
			context->hdrReliable.type = type;
			context->hdrReliable.index = ++numEmission;
			context->data = (char*)data;
			context->rlbnet = this;

			Thread * t = new Thread("send worker");
			t->Fork(ThreadSendHandler, (int) context);
			currentThread->Yield();

			break;
		}
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
	char * buffer = new char[MaxPacketSize];

    while(1)
    {
        // first, wait for a message
        messageAvailable->P();
        PacketHeader pktHeader = network->Receive(buffer);
        PacketHeaderReliable * pktHdrReliable = ParseReliableHeader(buffer);

        //printf("ReliableNet : Message reçu [%s] <%s>\n", pktHdrReliable->type == ACK ? "ACK" : (pktHdrReliable->type == MSG ? "MSG" : "ERROR"), buffer);
        printf("Reception d'un message [%s] #%d\n", pktHdrReliable->type == MSG ? "MSG" : "ACK", numEmission);

        switch(pktHdrReliable->type)
        {
        	case ACK:
        		if(pktHdrReliable->index == numEmission)
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
        		if(numReception == pktHdrReliable->index - 1 || numReception == pktHdrReliable->index)
        		{
        			if(numReception != pktHdrReliable->index)
        			{
	        			numReception = pktHdrReliable->index;

	        			Paquet * p = new Paquet(pktHeader, buffer);
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

    delete [] buffer;
}