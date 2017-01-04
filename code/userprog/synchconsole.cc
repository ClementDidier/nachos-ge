#ifdef CHANGED

#define EOL '\0'

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

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
}

//----------------------------------------------------------------------
// SynchConsole::SynchPutChar()
// 	Ecrit un caractère de façon synchrone dans la console simulée
//----------------------------------------------------------------------
void SynchConsole::SynchPutChar(const char ch)
{
	console->PutChar(ch);
	writeDone->P();
}

//----------------------------------------------------------------------
// SynchConsole::SynchGetChar()
// 	Obtient un caractère de façon synchrone depuis l'entrée standard
//  de la console simulée
//----------------------------------------------------------------------
char SynchConsole::SynchGetChar()
{
	readAvail->P();
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
	int i = 0;
	char c;

	// Tant que non fin de ligne et non fin de fichier
	while((c = s[i]) != EOL && c != EOF)
	{
		SynchPutChar(s[i]);
		i++;
	}
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
	int i = 0;
	char c;

	// Tant que non fin de ligne, non fin de fichier et inférieur à la taille donnée
	while((c = s[i]) != EOL && c != EOF && i < n)
	{
		s[i] = SynchGetChar();
		i++;
	}

	// Remplir le reste de la chaîne proprement
	while(i < n)
	{
		s[i] = EOL;
		i++;
	}
}

#endif // CHANGED