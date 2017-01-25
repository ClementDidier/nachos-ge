/**
 * \file shell.c
 * \brief affiche un shell qui permet de lancer d'autre programme utilisateur. Ne fonctionne pas sur le système de fichier
*/
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
PutString("                                     -o---..``\n");
PutString("                                     `:------:-..``\n");
PutString("                                      `.---..---:---.```\n");
PutString("                                       `-.........----:-.````\n");
PutString("                                       `..---..-..-...-------...`\n");
PutString("                  /+:..-.-.`````        `--...-........-..---:-:--..```\n");
PutString("                  /o/:-:/+:-:::::---....-://////:::::-//-----::::/:----.``\n");
PutString("                   ++:---:/::::::-.---.-----::/+///+++++/////+++++/:::::-..```\n");
PutString("                   ./-::--:::-----:-:---......----..-.::---:/ysoo+/:::::..:.-...```\n");
PutString("                    -/--:-:/:-----:--...----..-..----.--.-.:oyso//:----.......:+-..`\n");
PutString("                    `/:-:--::---:-:--.....--.....----....-+sso/:-:::...-..----/y/.`\n");
PutString("                     :+:--::::/----::.....----....---.../ss+/:-:---.......:::--.` \n");
PutString("                     `+/:-::::/::---:-.--..----..---::/syo/:---:--..--....--.`\n");
PutString("                      ./:--:-:/:--:---.--...---...--/hyo+:-:---:::-------.`` \n");
PutString("                      `::-----::----.-...----:----:+so+//-------:--::::.` \n");
PutString("                       .+:-:-:::::---::--------:++s+:-://-------:--/:.` \n");
PutString("                        +/---:-:::-------:----:/oo+:---::--:-::::+:.  \n");
PutString("                        `::--:---:/-----::---++:::::--:--:::-/:::- \n");
PutString("                         .-------:::-------/oo.`--:----:::--:s+-` \n");
PutString("                         `/:-------------:+/.`  .::-:-:-::://:`\n");
PutString("                          -/--:-:-----o+/-`      .--::::-/+:. \n");
PutString("                          `-:------/s+:.         `-::/+/::-` \n");
PutString("                           .///---::-`            :o+o-```\n");
PutString("                           `/+//:.`               `:-  \n");
PutString("                            -/:. \n");
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
    char buffer[buffersize];
    resetbuffer(buffer);
	while (buffer[0] != 'q'){
		PutString("\nuser@NachOS:~/ $ ");

		resetbuffer(buffer);

		GetString(buffer, 60);

		if (buffer[0] != '\0' && buffer[0] != '\n' && buffer[1] != '\0' && buffer[1] != ' '){ // c'est un programme
			ForkExec(buffer);
			SimpleShellProcJoin();
		}
		else{
			switch (buffer[0]){
      			case 'c':
				{
					clearscr();
					break; 
      			}
      			case 'h':
				{
					help();
					break; 
      			}
			}
		}
    }
    return 0;
}