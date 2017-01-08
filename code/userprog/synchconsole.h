/**
 * \file synchconsole.h
 * \brief Émulation d'une console
 *
*/

#ifdef CHANGED
#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"
/**
 * \class SynchConsole
 * \brief Émulation d'une console synchrone
*/
class SynchConsole
{

public:

	/**
	 * \fn 	SynchConsole::SynchConsole(char *readFile, char *writeFile)
	 * \brief Initialise l'utilisation d'une console hardware simulée
	 * \param readFile UNIX file simulating the keyboard (NULL -> use stdin)
	 * \param writeFile UNIX file simulating the display (NULL -> use stdout)
	*/
	SynchConsole(char *readFile, char *writeFile);
	/**
	 * \fn SynchConsole::~SynchConsole()
	 * \brief Nettoie la console simulée
	*/
	~SynchConsole();
	/**
	 * \fn void SynchConsole::SynchPutChar(const char ch)
	 * \brief Ecrit un caractère de façon synchrone dans la console simulée
	 * \param ch Caractere à afficher
	*/
	void SynchPutChar(const char ch);
	/**
	 * \fn char SynchConsole::SynchGetChar()
	 * \brief Obtient un caractère de façon synchrone depuis l'entrée standard de la
	 * console simulée
	 * \return Retourne un caractère entré dans la console
	*/
	char SynchGetChar();
	/**
	 * \fn void SynchConsole::SynchPutString(const char *s)
	 * \brief Écrit une chaîne de caractères de façon synchrone dans la console simulée
	 * \param s chaîne de caractères à flush sur la console
	*/
	void SynchPutString(const char *s);
	/**
	 * \fn void SynchConsole::SynchGetString(char *s, int n)
	 * \brief Obtient une chaîne de caractères de façon synchrone depuis l'entrée
	 * standard de la console simulée, Information : Ignore et écrase les données
	 * du buffer
	 * \param s chaîne de caractères resultante
	 * \param n taille maximale de la chaîne de caractères en entrée
	*/
	void SynchGetString(char *s, int n);
	/**
	 * \fn void SynchConsole::copyStringFromMachine(int from, char *to, unsigned size)
	 * \brief Converti une chaîne de caractères MIPS en chaîne de caractères Linux
	 * \param from L'adresse MIPS de la chaine de caractères (premier caractère)
	 * \param to Chaine de caractères valeur de retour
	 * \param size La taille maximale de la chaine de caractères à convertir
	*/
	void copyStringFromMachine(int from, char *to, unsigned size);
	/**
	 * \fn void SynchConsole::copyMachineFromString(char * from, int to, unsigned size)
	 * \brief Converti une chaîne de caractères Linux en chaîne de caractères MIPS
	 * \param from L'adresse de la chaine de caractères LINUX
	 * \param to L'adresse MIPS de la chaine de caractères
	 * \param size La taille maximale de la chaine de caractères à convertir
	*/
	void copyMachineFromString(char * from, int to, unsigned size);
	/**
	 * \fn void SynchConsole::SynchGetInt(int * n)
	 * \brief Obtenir un entier 8bits depuis l'entrée de la console standard.
	 * La fonction travail uniquement sur des entiers codés sur 8bits
	 * \param n L'entier resultant
	*/
	void SynchGetInt(int * n);
	/**
	 * \fn void SynchConsole::SynchPutInt(int n)
	 * \brief Ecrit un entier sur la console standard
	 * \param n L'entier à écrire
	*/
	void SynchPutInt(int n);

private:
	Console *console;
};

#endif // SYNCHCONSOLE_H
#endif // CHANGED
