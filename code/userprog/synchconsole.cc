#ifdef CHANGED

#define EOL '\0'
#define NL '\n'

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *semRead, *semWrite;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// SynchConsole::SynchConsole
// 	Initialise l'utilisation d'une console hardware simulée
//
//	"readFile" -- UNIX file simulating the keyboard (NULL -> use stdin)
//	"writeFile" -- UNIX file simulating the display (NULL -> use stdout)
//----------------------------------------------------------------------
SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	semRead = new Semaphore("read console", 1);
	semWrite = new Semaphore("write console", 1);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}

//----------------------------------------------------------------------
// SynchConsole::~SynchConsole
// 	Nettoie la console simulée
//----------------------------------------------------------------------
SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
	delete semRead;
	delete semWrite;
}

//----------------------------------------------------------------------
// SynchConsole::SynchPutChar()
// 	Ecrit un caractère de façon synchrone dans la console simulée
//----------------------------------------------------------------------
void SynchConsole::SynchPutChar(const char ch)
{
	console->PutChar(ch);
	writeDone->P(); 
	// Ecriture réalisée, on peux continuer
}

//----------------------------------------------------------------------
// SynchConsole::SynchGetChar()
// 	Obtient un caractère de façon synchrone depuis l'entrée standard
//  de la console simulée
//----------------------------------------------------------------------
char SynchConsole::SynchGetChar()
{
	readAvail->P(); 
	// Lecture possible
	return console->GetChar();
}

//----------------------------------------------------------------------
// SynchConsole::SynchPutString()
// 	Ecrit une chaîne de caractères de façon synchrone
//  dans la console simulée
//	s : chaîne de caractères à flush sur la console
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// SynchConsole::SynchGetString()
// 	Obtient une chaîne de caractères de façon synchrone depuis l'entrée
//  standard de la console simulée
//	s : chaîne de caractères resultante
//	n : taille maximale de la chaîne de caractères en entrée
//----------------------------------------------------------------------
void SynchConsole::SynchGetString(char *s, int n)
{
	semRead->P();

	int i = 0;

	// Tant que inférieur à la taille donnée
	while(i < n)
	{
		s[i] = SynchGetChar();

		if(s[i] == EOF || s[i] == NL)
		{

			break;
		}

		i++;
	}

	// Remplir le reste de la chaîne proprement
	while(i < n)
	{
		s[i] = EOL;
		i++;
	}

	semRead->V();
}

//----------------------------------------------------------------------
// copyStringFromMachine()
// 	Converti une chaîne de caractères MIPS en chaîne de caractères Linux
//	from : L'adresse MIPS de la chaine de caractères (premier caractère)
//	to : Chaine de caractères valeur de retour
//	size : La taille maximale de la chaine de caractères à convertir
//----------------------------------------------------------------------
void SynchConsole::copyStringFromMachine(int from, char *to, unsigned size)
{
	// Lecture en mémoire de la chaine de caractères

	char c;
	int v = 0;
	unsigned int i = 0;
	while(i < size)
	{
		machine->ReadMem(from + i, 1, &v);
		c = (char)v;

		if(c == EOF)
			break;
		
		*(to + i) = c;

		i++;
	}

	*(to + size) = '\0';
}

//----------------------------------------------------------------------
// copyMachineFromString()
// 	Converti une chaîne de caractères Linux en chaîne de caractères MIPS
//	from : L'adresse de la chaine de caractères LINUX
//	to : L'adresse MIPS de la chaine de caractères
//	size : La taille maximale de la chaine de caractères à convertir
//----------------------------------------------------------------------
void SynchConsole::copyMachineFromString(char * from, int to, unsigned size)
{
	char c = 'a';
	unsigned int i = 0;
	while(i < size && c != EOL && c != EOF && c != NL)
	{
		c = from[i];
		machine->WriteMem(to + i, 1, (int)c);
		i++;
	}
}

#endif // CHANGED