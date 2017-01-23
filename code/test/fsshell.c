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

void updatecp(char * currentPath, char * buffer){
	int i;
	int m;
	if(buffer [2] == '.' && buffer[3] == '.'){
		m = 0;
		for(i = 0; i<buffersize-1; i++){
			if (currentPath[i] == '/' && currentPath[i+1] != '\0'){
				m = i;
			}
		}
		for(i = m+1; i<buffersize; i++){
			currentPath[i] = '\0';
		}
	}
	else if(buffer [2] != '.'){
		for(i = 0; i<buffersize; i++){
			if (currentPath[i] != '\0'){
				m = i;
			}
		}
		m++;
		i = 0;
		while(m < buffersize-1 && buffer[2+i] != '\0'){
			currentPath[m] = buffer[2+i];
			m++;
			i++;
		}
		currentPath[m] = '/';
	}
}


int
main ()
{
	char currentPath[buffersize];
	clearscr();
	PutString("┌─────────────────────");
	PutString("─────────────────────┐\n");
	PutString("│╔═╗┬┌┬┐┌─┐┬  ┌─┐  ┌─┐┌─┐   ╔═╗┬ ┬┌─┐┬  ┬  │\n");
	PutString("│╚═╗││││├─┘│  ├┤   ├┤ └─┐───╚═╗├─┤├┤ │  │  │\n");
	PutString("│╚═╝┴┴ ┴┴  ┴─┘└─┘  └  └─┘   ╚═╝┴ ┴└─┘┴─┘┴─┘│\n");
	PutString("├────────────────────");
	PutString("──────────────────────┤\n");
	PutString("│ l ....... affiche le repertoire courrant │\n");
	PutString("│ c <dir. name> ..... change de repertoire │\n");
	PutString("│ m <directory name> . créer un repertoire │\n");
	PutString("│ n <file name> ......... créer un fichier │\n");
	PutString("│ r <dir. name> ... supprime un repertoire │\n");
	PutString("│ d <file name> ...... supprime un fichier │\n");
	PutString("│ q .............................. quitter │\n");
	PutString("└───────────────────────");
	PutString("───────────────────┘\n");
	char buffer[buffersize];
	resetbuffer(currentPath);
	resetbuffer(buffer);
	currentPath[0] = '/';
	while (!(buffer[0] == 'q' && buffer[1] == '\0')){
		PutString("\nuser@NachOS:~");
		PutString(currentPath);
		PutChar('$');
		resetbuffer(buffer);
		GetString(buffer, 60);

		switch(buffer[0]){
			case 'm':
				if(buffer[1] == ' '){
					Mkdir(&buffer[2]);
				}
			break;
			case 'l':
				if(buffer[1] == '\0'){
					List();
				}
			break;
			case 'n':
				if(buffer[1] == ' '){
					Create(&buffer[2]);
				}
			break;
			case 'c':
				if(buffer[1] == ' '){
					if(currentPath[1] == '\0' && buffer[3] == '.'){
						PutString("Opération Impossible\n");
					}
					else{
						if(ChangeDirectory(&buffer[2])){
							updatecp(currentPath,buffer);
						}
					}
				}
			break;
			case 'r':
				if(buffer[1] == ' '){
					RmDir(&buffer[2]);
				}
			break;
			case 'd':
				if(buffer[1] == ' '){
					Remove(&buffer[2]);
				}
			break;
		}
	}
	return 0;
}