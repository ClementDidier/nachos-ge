#include "syscall.h"

int
main ()
{
    char buffer[60];
    int i;

    for(i = 0; i < 60; i++){
		buffer[i] = '\0';
	}

	PutString("Console utilisateur simple, q pour quitter : \n");
	PutString("Veuillez entrer un executable (dans \\test\\) : \n");

	while (1){
		PutString("\n->");

		for(i = 0; i < 60; i++){
			buffer[i] = '\0';
		}

		GetString(buffer, 60);

		if (buffer[0] == 'q' && buffer[1] == '\0')
		{
			Halt();
		}
		else{
			if(buffer[0] != '\0' && buffer[0] != '\n')
			{
				ForkExec(buffer);
			}
		}
    }
}
