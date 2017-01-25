#ifndef RELIABLENET_H
#define RELIABLENET_H

#include "network.h"
#include "synchlist.h"

#define MSG 46 //Paquet contenant un message
#define ACK 47 //Paquet contenant un auquitement
#define DATA_SIZE 30 //Taille des données d'un paquet
#define MAXREEMISSIONS 10 //nombre maximum de réémission d'un même paquet
#define TEMPO 2 //Délai entre chaque réémission de paquet

//Header ReliableNet
typedef struct 
{
	char type;
	int index;
} PacketHeaderReliable;



class ReliableNet
{
	public:
		/**
		 * \fn ReliableNet(NetworkAddress addr, double reliability, NetworkAddress target)
		 * \brief Constructeur de la classe ReliableNet
		 * \param addr Adresse réseau de la machine
		 * \param reliability Fiabilité du réseau
		 * \param target Adresse réseau de la machine cible
		 */
		ReliableNet(NetworkAddress addr, double reliability, NetworkAddress target);
		/**
		 * \fn ReliableNet()
		 * \brief Destructeur de la classe ReliableNet
		 */
		~ReliableNet();


		/**
		 * \fn void Send(char type, const char *data, int size)
		 * \brief Envoie un message à la machine cible
		 * \param type Type du paquet à envoyer
		 * \param data Contenu du message
		 * \param size Taille du message
		 */
		void Send(char type, const char *data, int size);

		/**
		 * \fn int Receive(char *data, int size)
		 * \brief Récupére les messages reçu
		 * \param data buffer de récupération
		 * \param size Taille du buffer de récupération
		 * \return 0 si tout les messages ont été récupéré, 1 si il en reste encore
		 */
		int Receive(char *data, int size);

		/**
		 * \fn void PacketSent()
		 * \brief Gére le sémaphore d'envoie
		 */
		void PacketSent();		// Interrupt handler, called when outgoing 
				// packet has been put on network; next 
				// packet can now be sent

		/**
		 * \fn void IncomingPacket()
		 * \brief Gére le sémaphore de récéption
		 */
    	void IncomingPacket();	// Interrupt handler, called when incoming
   				// packet has arrived and can be pulled
				// off of network (i.e., time to call 
				// PostalDelivery)

    	/**
		 * \fn void WaitMessages()
		 * \brief Attend les messages
		 */
    	void WaitMessages();

    	int numEmission;//numéro du dernier paquet envoyé
    	int numReception;//numéro du dernier paquet reçu
    	int numAquitement;//numéro du dernier paquet aquité
    	Semaphore * messageAvailable;// V'ed when message has arrived from network
    	Semaphore * messageSent;	// V'ed when next message can be sent to network
    	Semaphore * semAck;//bloque l'envoie de nouveaux paquet tant que le précédent n'est pas aquité
    	Lock *sendLock;		// Only one outgoing message at a time
    	Network* network;

	private:
		PacketHeaderReliable * ParseReliableHeader(char * buffer);//parse le header
		
		SynchList * messages;//liste des messages reçu
		
		NetworkAddress from;
		NetworkAddress target;

    	char memory[MaxPacketSize];
    	int memoryIndex;
    	int memorySize;

    	

    	
};

//structure fournissant les arguments aux threads gérant les paquets
typedef struct
{
	PacketHeader hdr;
	PacketHeaderReliable hdrReliable;
	char* data;
	ReliableNet * rlbnet;
} PacketContext;

#endif

