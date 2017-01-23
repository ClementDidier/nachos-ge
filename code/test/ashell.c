#include "syscall.h"

#define buffersize 60

void resetbuffer(char * buffer){
	int i;
	for (i = 0; i < buffersize; i++){
		buffer[i] = '\0';
	}
}

void clearscr(){
	int i;
	for(i = 0; i<15; i++){
		PutString("\n\n\n\n\n\n\n\n\n\n");
	}
}

void help(){
	PutString("          ╔════════════════════");
    PutString("═════════════════════════════");
    PutString("══════════");
    PutString("═══════════════════╗\n");
    PutString("          ║                                                                              ║\n");
	PutString("          ║     --- Console utilisateur simple ---, commandes disponibles:               ║\n");
	PutString("          ║     q : quitter                                                              ║\n");
	PutString("          ║     h : affiche l'aide                                                       ║\n");
	PutString("          ║     c : vider l'affichage                                                    ║\n");
	PutString("          ║     m <directory> : créer un répertoire                                      ║\n");
	PutString("          ║     f <filename> : créer un fichier                                          ║\n");
	PutString("          ║     d <directory> : changer de repertoire courrant                           ║\n");
	PutString("          ║     l : affiche le repertoire courrant                                       ║\n");
	PutString("          ║     x : copier le fichier (UNIX) dans le repertoire (NachOS) courrant        ║\n");
	PutString("          ║     <executable> ( nom >= 2 caractères sans arguments )                      ║\n");
	PutString("          ║                                                                              ║\n");
	PutString("          ╚════════════════════");
	PutString("═════════════════════════════");
	PutString("══════════");
	PutString("═══════════════════╝\n");
}

int
main ()
{
	clearscr();
	PutString("   _   _            _     _____ _____     ");
	PutString(" _____ _                 _      _____ _          _ _ \n");
	PutString("  | \\ | |          | |   |  _  /  ___|    ");
	PutString("/  ___(_)               | |    /  ___| |        | | |\n");
	PutString("  |  \\| | __ _  ___| |__ | | | \\ `--.     ");
	PutString("\\ `--. _ _ __ ___  _ __ | | ___\\ `--.| |__   ___| | |\n");
	PutString("  | . ` |/ _` |/ __| '_ \\| | | |`--. \\    ");
	PutString(" `--. \\ | '_ ` _ \\| '_ \\| |/ _ \\`--. \\ '_ \\ / _ \\ | |\n");
	PutString("  | |\\  | (_| | (__| | | \\ \\_/ /\\__/ /    ");
	PutString("/\\__/ / | | | | | | |_) | |  __/\\__/ / | | |  __/ | |\n");
	PutString("  \\_| \\_/\\__,_|\\___|_| |_|\\___/\\____/     ");
	PutString("\\____/|_|_| |_| |_| .__/|_|\\___\\____/|_| |_|\\___|_|_|\n");
	PutString("                                          ");
	PutString("                  | |\n");
	PutString("                                          ");
	PutString("                  |_|\n");

	help();

	return 0;
}