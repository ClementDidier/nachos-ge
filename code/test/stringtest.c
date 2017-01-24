/**
 * \file stringtest.c
 * \brief Récupère des données de l'utilisateur via GetInt et affiche un (ou plusieurs)
 * train(s) grace a l'appel de PutString
 * La vitesse des train est très dépendante de la vitesse CPU (n'ayant pas d'horloge interne)...
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

void CustomUsleep(int duree){
	int i = 0;
	while(i < duree*2000){
	  i++;
	}
}

int main()
{
	int i = 0;
	int k = 0;
	char rails[largeur+1];

	int j;
	char clearscr[hauteur+1];

	char deplacement[largeur+1];

	int choix;
	int vitesse;

	PutString(clearscr);
	PutString("Veuillez entrer le nombre de train (1-10) = ");
	GetInt(&choix);
	PutString("Veuillez entrer une vitesse 1-3 = ");
	GetInt(&vitesse);

	for(j = 0; j<hauteur; j++){
		clearscr[j] = '\n';
	}
	clearscr[hauteur] = '\0';

	for(k = 1; k<=choix; k++){

		for(j = 0 ; j<largeur ; j++){
			rails[j] = '=';
			deplacement[j] = '\0';
		}

		for(i = 0; i<largeur; i++){
			deplacement[i] = ' ';
			PutString(clearscr);
			PutString(clearscr);
			CustomUsleep(1);
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
			CustomUsleep((4-vitesse)*7);
		}
	}
	PutString(clearscr);
	return 0;
}
