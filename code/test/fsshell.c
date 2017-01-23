#include "syscall.h"

#define buffersize 128

void resetbuffer(char * buffer){
	int i;
	for (i = 0; i < buffersize; i++){
		buffer[i] = '\0';
	}
}

void onecp(char * currentPath, char * tempPath){
	int i;
	int m;
	if(tempPath [0] == '.' && tempPath[1] == '.'){
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
	else if(tempPath [0] != '.'){
		for(i = 0; i<buffersize; i++){
			if (currentPath[i] != '\0'){
				m = i;
			}
		}
		m++;
		i = 0;
		while(m < buffersize-1 && tempPath[i] != '\0'){
			currentPath[m] = tempPath[i];
			m++;
			i++;
		}
		currentPath[m] = '/';
	}
}

void updatecp(char * currentPath, char * buffer){
	int i = 0;
	int current = 0;
	int cPos = 0;
	char tempPath[buffersize];

	resetbuffer(tempPath);

	if(buffer[2] == '/'){
		resetbuffer(currentPath);
		i = 2;
		while((i < buffersize-1) && buffer[i] != '\0'){
			if((buffer[i] != '/' && i < buffersize-1 && buffer[i+1] == '\0')){
				currentPath[i-1] = '/';
				currentPath[i-2] = buffer[i];
			}
			else{
				currentPath[i-2] = buffer[i];
			}
			i++;
		}
	}
	else{
		current = 2;
		while(current < buffersize && buffer[current] != '\0'){
			if(buffer[current] == '/'){
				onecp(currentPath,tempPath);
				cPos = 0;
			}
			else{
				tempPath[cPos] = buffer[current];
				cPos++;
			}
			current++;
		}
		if(cPos != 0){
			onecp(currentPath,tempPath);
		}
	}	
}


int
main ()
{
	char currentPath[buffersize];
	int i;
	for(i = 0; i<15; i++){
		PutString("\n\n\n\n\n\n\n\n\n\n");
	}
	PutString(" ╔═╗┬┌┬┐┌─┐┬  ┌─┐  ╔═╗╔═╗  ╔═╗┬ ┬┌─┐┬  ┬\n");
	PutString(" ╚═╗││││├─┘│  ├┤   ╠╣ ╚═╗  ╚═╗├─┤├┤ │  │\n");
	PutString(" ╚═╝┴┴ ┴┴  ┴─┘└─┘  ╚  ╚═╝  ╚═╝┴ ┴└─┘┴─┘┴─┘\n");
	PutString(" l ....... affiche le repertoire courrant\n");
	PutString(" c <dir. name> ..... change de repertoire\n");
	PutString(" m <directory name> . créer un repertoire\n");
	PutString(" n <file name> ......... créer un fichier\n");
	PutString(" r <dir. name> ... supprime un repertoire\n");
	PutString(" d <file name> ...... supprime un fichier\n");
	PutString(" q .............................. quitter\n");
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
					if(ChangeDirectory(&buffer[2])){
						updatecp(currentPath,buffer);
					}
					else{
						PutString("\nRepertoire invalide => home\n");
						ChangeDirectory("/");
						resetbuffer(currentPath);
						currentPath[0] = '/';
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