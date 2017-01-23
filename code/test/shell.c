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

int
main ()
{
	ForkExec("ashell");
	SimpleShellProcJoin();
    char buffer[buffersize];
	while (1){
		PutString("\nuser@NachOS:~/ $ ");

		resetbuffer(buffer);

		GetString(buffer, 60);

		if (buffer[0] != '\0' && buffer[0] != '\n' && buffer[1] != '\0' && buffer[1] != ' '){ // c'est un programme
			ForkExec(buffer);
			SimpleShellProcJoin();
		}
		else{
			switch (buffer[0]){
				case 'q':
				{
					Halt();
					break; // jamais atteint
      			}
      			case 'c':
				{
					clearscr();
					break; 
      			}
      			case 'h':
				{
					ForkExec("ashell");
					SimpleShellProcJoin();
					break; 
      			}
      			case 'l':
				{
					List();
					break; 
      			}
      			case 'm':
				{
					List();
					break; 
      			}
      			case 'd':
				{
					List();
					break; 
      			}
      			case 'f':
				{
					List();
					break; 
      			}
      			case 'x':
				{
					//TODO
					break; 
      			}
			}
		}
    }
}