/**
 * \file synchconsole.cc
 * \brief Émulation d'une console
*/

#define EOL '\0'
#define NL '\n'

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *semRead, *semWrite, *semMemory, *semPutChar;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }


SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	semRead = new Semaphore("read console", 1);
	semWrite = new Semaphore("write console", 1);
	semMemory = new Semaphore("memory", 1); // Semaphore de protection de la zone critique mémoire (ReadMem / WriteMem)
	semPutChar = new Semaphore("putchar lock", 1); // Semaphore de protection de la zone critique de la méthode PutChar
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}


SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
	delete semRead;
	delete semWrite;
	delete semPutChar;
	delete semMemory;
}

void SynchConsole::SynchPutChar(const char ch)
{
	semPutChar->P();

	console->PutChar(ch);
	writeDone->P();
	// Ecriture réalisée, on peux continuer

	semPutChar->V();
}

char SynchConsole::SynchGetChar()
{
	readAvail->P();
	// Lecture possible
	char ch = console->GetChar();
	return ch;
}

void SynchConsole::SynchPutString(const char s[])
{
	semRead->P();
	semWrite->P();

	int i = 0;
	char c;

	// Tant que non fin de ligne et non fin de fichier
	while((c = s[i]) != EOL && c != EOF)
	{
		SynchPutChar(s[i]);
		i++;
	}


	semWrite->V();
	semRead->V();
}

void SynchConsole::SynchGetString(char *s, int n)
{

	semRead->P();

	int i = 0;
	char c;

	// Tant que inférieur à la taille donnée
	while(i < n)
	{
		// Lecture du caractère en entrée
		c = SynchGetChar();

		// Si le caractère lu est EOF ou '\n'
		if(c == EOF || c == NL)
		{
			// On arrête de lire l'entrée
			break;
		}

		// Si bon caractère, on l'ajoute à la chaîne de caratère resultante
		s[i] = c;

		i++;
	}

	// Caractere de fin de chaîne
	s[i] = EOL;

	semRead->V();
}

void SynchConsole::copyStringFromMachine(int from, char *to, unsigned size)
{
	// Lecture en mémoire de la chaine de caractères
	semMemory->P();

	char c;
	int v = 0;
	unsigned int i = 0;

	while(i < size)
	{
		machine->ReadMem(from + i, 1, &v);
		c = (char)v;

		if(c == EOL || c == EOF)
			break;

		to[i] = c;

		i++;
	}

	to[i] = '\0';

	semMemory->V();
}

void SynchConsole::copyMachineFromString(char * from, int to, unsigned size)
{
	semMemory->P();

	char c;
	unsigned int i = 0;
	while(i < size && (c = from[i]) != EOL && c != EOF && c != NL)
	{
		machine->WriteMem(to + i, 1, (int)c);
		i++;
	}

	semMemory->V();
}

void SynchConsole::SynchPutInt(int n)
{
	char buffer[MAX_STRING_SIZE];
	snprintf(buffer, MAX_STRING_SIZE, "%d", n);

	SynchPutString(buffer);
}

void SynchConsole::SynchGetInt(int * n)
{
	char buffer[MAX_STRING_SIZE];
	SynchGetString(buffer, MAX_STRING_SIZE);
	sscanf(buffer, "%d", n);

	if(*n > 255) // ASSERTION avec message : Entier codé uniquement sur 8bits
	{
		SynchPutString("Erreur : Entier non compris entre [0; 255]\n");
		ASSERT(FALSE);
	}
}
