#ifdef CHANGED

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
	semMemory = new Semaphore("memory", 1); // Semaphore de protection de la zone critique mémoire (ReadMem / WriteMem)
	semPutChar = new Semaphore("putchar lock", 1); // Semaphore de protection de la zone critique de la méthode PutChar
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
	delete semPutChar;
}

//----------------------------------------------------------------------
// SynchConsole::SynchPutChar()
// 	Ecrit un caractère de façon synchrone dans la console simulée
//----------------------------------------------------------------------
void SynchConsole::SynchPutChar(const char ch)
{
	semPutChar->P();

	console->PutChar(ch);
	writeDone->P(); 
	// Ecriture réalisée, on peux continuer

	semPutChar->V();
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
//	Information : Ignore et écrase les données du buffer
//	s : chaîne de caractères resultante
//	n : taille maximale de la chaîne de caractères en entrée
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// copyMachineFromString()
// 	Converti une chaîne de caractères Linux en chaîne de caractères MIPS
//	from : L'adresse de la chaine de caractères LINUX
//	to : L'adresse MIPS de la chaine de caractères
//	size : La taille maximale de la chaine de caractères à convertir
//----------------------------------------------------------------------
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


//----------------------------------------------------------------------
// PutInt()
// 	Ecrit un entier sur la console standard
//	n : L'entier à écrire
//----------------------------------------------------------------------
void SynchConsole::SynchPutInt(int n)
{
	char buffer[MAX_STRING_SIZE];
	snprintf(buffer, MAX_STRING_SIZE, "%d", n);

	SynchPutString(buffer);
}

//----------------------------------------------------------------------
// GetInt()
// 	Obtenir un entier 8bits depuis l'entrée de la console standard
//	n : L'entier resultant
//	Info : La fonction travail uniquement sur des entiers codés sur 8bits
//----------------------------------------------------------------------
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

#endif // CHANGED