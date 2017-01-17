/**
 * \file stringtest.c
 * \brief Récupère des données de l'utilisateur via GetInt et affiche un (ou plusieurs)
 * train(s) grace a l'appel de PutString
 */

#include "syscall.h"

#define largeur 100
#define taille 68
#define hauteur 70
#define ligne1 "                                                  _,-'-._           \n"
#define ligne2 "                                                 <_     _>          \n"
#define ligne3 "     _____----'----________________________________`---'_______     \n"
#define ligne4 "    /----------------------------------------------------------\\   \n"
#define ligne5 "   /] [_] #### [___] #### [___]  \\/  [___] #### [___] #### [_] [\\ \n"
#define ligne6 "  /----------------------------11407-----------------------|-|---\\ \n"
#define ligne7 "  |=          S  B  B                          C  F  F     |_|  =|  \n"
#define ligne8 "[=|______________________________________________________________|=]\n"
#define ligne9 "   )/_-(-o-)=_=(=8=)=_=(-o-)-_ [____] _-(-o-)=_=(=8=)=_=(-o-)-_\\(  \n"

void waitloop(int n){
	int j = 0;
	while(j<n){
		j++;
	}
}

int main()
{
	int i = 0;
	int k = 0;
	char rails[largeur];

	int j;
	char clearscr[hauteur];

	char deplacement[largeur];

	int choix;
	int vitesse;

	PutString(clearscr);
	PutString("Veuillez entrer le nombre de train (1-10) = ");
	GetInt(&choix);
	PutString("Veuillez entrer une vitesse 1-2 = ");
	GetInt(&vitesse);

	for(k = 1; k<=choix; k++){

		for(j = 0; j<hauteur; j++){
			clearscr[j] = '\n';
		}

		for(j = 0 ; j<largeur ; j++){
			rails[j] = '=';
			deplacement[j] = '\0';
		}

		for(i = 0; i<largeur; i++){
			deplacement[i] = ' ';
			PutString(clearscr);
			waitloop(2000);
			PutString(deplacement);
			PutString(ligne1);
			PutString(deplacement);
			PutString(ligne2);
			PutString(deplacement);
			PutString(ligne3);
			PutString(deplacement);
			PutString(ligne4);
			PutString(deplacement);
			PutString(ligne5);
			PutString(deplacement);
			PutString(ligne6);
			PutString(deplacement);
			PutString(ligne7);
			PutString(deplacement);
			PutString(ligne8);
			PutString(deplacement);
			PutString(ligne9);
			PutString(rails);
			PutString(rails);
			waitloop((4-vitesse)*10000);
		}
	}
	PutString(clearscr);
	return 0;
}
