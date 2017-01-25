/**
 * \file fsshell.c
 * \brief affiche un shell pour le système de fichier permettant des opérations basique sur les fichiers/repertoire
 * Ce fichier doit être copier sur le système de fichier nachos et il sera nécessaire de lancer nachos avec le flavor userprog-fs
*/

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
				resetbuffer(tempPath);
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
	OpenFileId OFile1;
	OpenFileId OFile2;

	char currentPath[buffersize];
	char buffer[buffersize];
	char bufferRead[buffersize];

	resetbuffer(currentPath);
	resetbuffer(buffer);

	PutString("\n\nSimple FS Shell\n q : quitter\n l : affiche le rep.\n c <dir.> : change rep.");
	PutString("ertoire\n m <dir.> : créer repertoire\n n <file> : créer fichier\n r <dir.");
	PutString("> : supprime repertoire\n d <file> : supprime fichier\n o <file> : Ouvre écrit");
	PutString(" fichier1\n p <file> : Ouvre écrit fichier2\n f <file> : Lit ferme fichier1");
	PutString("\n g <file> : Lit ferme fichier2\n");

	
	
	currentPath[0] = '/';
	while (!(buffer[0] == 'q' && buffer[1] == '\0')){
		PutString("\nuser@NachOS:~");
		PutString(currentPath);
		PutChar('$');
		resetbuffer(buffer);
		resetbuffer(bufferRead);
		GetString(buffer, 60);

		switch(buffer[0]){
			case 'm':
				Mkdir(&buffer[2]);
			break;
			case 'l':
				List();
			break;
			case 'n':
				Create(&buffer[2]);
			break;
			case 'c':
				if(ChangeDirectory(&buffer[2])){
					updatecp(currentPath,buffer);
				}
				else{
					PutString("\nRep. Invalide => home\n");
					ChangeDirectory("/");
					resetbuffer(currentPath);
					currentPath[0] = '/';
				}
			break;
			case 'r':
				RmDir(&buffer[2]);
			break;
			case 'd':
				Remove(&buffer[2]);
			break;
			case 'o':
				OFile1 = Open(&buffer[2]);
				PutString("Ecriture-1\n>");
				resetbuffer(buffer);
				SetCursor(OFile1,0);
				GetString(buffer, 60);
				Write(buffer,42,OFile1);
				resetbuffer(buffer);
			break;
			case 'f':
				PutString("Lecture-1\n>");
				SetCursor(OFile1,0);
				Read(bufferRead,42,OFile1);
				PutString(bufferRead);
				PutChar('\n');
				Close(&buffer[2]);
			break;
			case 'p':
				OFile2 = Open(&buffer[2]);
				PutString("Ecriture-2\n>");
				resetbuffer(buffer);
				SetCursor(OFile2,0);
				GetString(buffer, 60);
				Write(buffer,42,OFile2);
				resetbuffer(buffer);
			break;
			case 'g':
				PutString("Lecture-2\n>");
				SetCursor(OFile2,0);
				Read(bufferRead,42,OFile2);
				PutString(bufferRead);
				PutChar('\n');
				Close(&buffer[2]);
			break;
		}
	}
	return 0;
}