#ifndef FORK_H
#define FORK_H

#include "system.h"

/**
 * \fn int ForkExec(char *s)
 * \brief Créer un thread système et lance le programme donné avec le thread crée
 * \param s Le nom du fichier executable à lancer
 */
int ForkExec(char* s);

void ProcessExit();

#endif //FORK_H