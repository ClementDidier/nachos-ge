#ifndef FORK_H
#define FORK_H

#include "system.h"

/**
 * \fn int ForkExec(char *s)
 * \brief Créer un thread système dans un nouveau processus et lance le programme donné avec le thread crée
 * \param s Le nom du fichier executable à lancer
 */
int ForkExec(char* s);

/**
 * \fn int ProcessExit()
 * \brief Appelé à la fin d'un processus, termine celui-ci et éteint la machine 
 *	s'il s'agit du dernier procesus présent sur la machine
 */
void ProcessExit();

#endif //FORK_H