/**
 * \file tetris.c
 * \brief permet de tester l'utilisation de la console (get char, get string, put string etc.), 
 *	la gestion et la synchronisation des threads utilisateurs, 
 *	les semaphores utilisateurs.
 *	Permet aussi de jouer au tetris 
 *	(note il est possible d'entrer jusqu'a 2 fonctions de déplacement, par exemple 1+2+enter vas faire aller la pièce à gauche puis va la faire pivoter)
*/

#include "syscall.h"

#define hauteur 40
#define largeur 20
#define milieu 9 // on commence à 0
#define coeff 2 // largeur/2
#define scrheight 70
#define nbPieceList 14

enum PieceType {CARRE, BARRE, LGAUCHE, LDROIT, T, QUATRE, QINVERSE};

enum PieceType current_piece;
int current_position;
char global[hauteur][largeur+1]; // hauteur largeur
char clrscrArray[scrheight];
int score = 0;
enum PieceType PieceList[nbPieceList];
int goKillYourself = 0;
int hasChanged = 1;
int askedAction[2];
sem_t * sem_action;
sem_t * sem_run;
int numPiece;
int previousscore = 0;

void CustomUsleep(int duree){
	int i = 0;
	while(i < duree*2000){
		i++;
	}
}


// retourne la colonne dans laquelle on a trouvé un 0 (en parcourant la ligne)
int findZInLine(int ligne){
	int j;
	for(j = 0; j<largeur; j++){
		if(global[ligne][j] == '0'){
			return j;
		}
	}
	return -1;
}

int findZInCol(int col){
	int i;
	for(i = 0; i<hauteur; i++){
		if(global[i][col] == '0'){
			return i;
		}
	}
	return -1;
}

int pivoterT(){
	int i = 0;
	int j = 0;

	while(i < hauteur && (findZInLine(i) == -1) ){
		i++;
	}

	while(j < largeur && (findZInCol(j) == -1) ){
		j++;
	}


	if(current_position == 0){ // T
		if(i < 2|| i > hauteur - 2){
			return -2;
		}

		if(j < 0 || j > largeur - 6){
			return -3;
		}

		if((
			global[i-1][j+2] == 'O')||(
			global[i-1][j+3] == 'O')||(
			global[i-2][j+2] == 'O')||(
			global[i-2][j+3] == 'O'
		))
		{
			return -4;
		}

		global[i][j+4] = ' ';
		global[i+1][j+4] = ' ';
		global[i][j+5] = ' ';
		global[i+1][j+5] = ' ';

		global[i-1][j+2] = '0';
		global[i-1][j+3] = '0';
		global[i-2][j+2] = '0';
		global[i-2][j+3] = '0';

		current_position = 1;

		return 1;
		
	}

	else if(current_position == 1){

		if(i < 0|| i > hauteur - 6){
			return -2;
		}

		if(j < 0 || j > largeur - 6){
			return -3;
		}

		if((
			global[i+2][j+4] == 'O')||(
			global[i+2][j+5] == 'O')||(
			global[i+3][j+4] == 'O')||(
			global[i+3][j+5] == 'O'
		))
		{
			return -4;
		}

		global[i+4][j+2] = ' ';
		global[i+4][j+3] = ' ';
		global[i+5][j+2] = ' ';
		global[i+5][j+3] = ' ';

		global[i+2][j+4] = '0';
		global[i+2][j+5] = '0';
		global[i+3][j+4] = '0';
		global[i+3][j+5] = '0';

		current_position = 2;
	}

	else if(current_position == 2){
		
		if(i < 0|| i > hauteur - 6){
			return -2;
		}

		if(j < 0 || j > largeur - 4){
			return -3;
		}

		if((
			global[i+4][j+2] == 'O')||(
			global[i+4][j+3] == 'O')||(
			global[i+5][j+2] == 'O')||(
			global[i+5][j+3] == 'O'
		))
		{
			return -4;
		}

		global[i+2][j] = ' ';
		global[i+2][j+1] = ' ';
		global[i+3][j] = ' ';
		global[i+3][j+1] = ' ';

		global[i+4][j+2] = '0';
		global[i+4][j+3] = '0';
		global[i+5][j+2] = '0';
		global[i+5][j+3] = '0';

		current_position = 3;

	}

	else if(current_position == 3){

		if(i < 0|| i > hauteur - 4){
			return -2;
		}

		if(j < 2 || j > largeur - 2){
			return -3;
		}

		if((
			global[i+2][j-1] == 'O')||(
			global[i+2][j-2] == 'O')||(
			global[i+3][j-1] == 'O')||(
			global[i+3][j-2] == 'O'
		))
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';

		global[i+2][j-1] = '0';
		global[i+2][j-2] = '0';
		global[i+3][j-1] = '0';
		global[i+3][j-2] = '0';

		current_position = 0;
		
	}

	return 0;
}

int pivoterLGauche(){
	int i = 0;
	int j = 0;

	while(i < hauteur && (findZInLine(i) == -1) ){
		i++;
	}

	while(j < largeur && (findZInCol(j) == -1) ){
		j++;
	}

	if (current_position == 0){ //_| -> |_

		if((j > largeur - 6) || (j < 0 )){

			return -2;
		}

		if((i > hauteur - 6) || (i < 0 )){

			return -3;
		}

		if((
			global[i][j] == 'O')||(
			global[i][j+1] == 'O')||(
			global[i+1][j] == 'O')||(
			global[i+1][j+1] == 'O')||(
			global[i+2][j] == 'O')||(
			global[i+2][j+1] == 'O')||(
			global[i+3][j] == 'O')||(
			global[i+3][j+1] == 'O')||(
			global[i+2][j+4] == 'O')||(
			global[i+2][j+5] == 'O')||(
			global[i+3][j+4] == 'O')||(
			global[i+3][j+5] == 'O')
		)
		{
			return -4;
		}

		global[i][j+2] = ' ';
		global[i][j+3] = ' ';
		global[i+1][j+2] = ' ';
		global[i+1][j+3] = ' ';
		global[i+4][j] = ' ';
		global[i+4][j+1] = ' ';
		global[i+4][j+2] = ' ';
		global[i+4][j+3] = ' ';
		global[i+5][j] = ' ';
		global[i+5][j+1] = ' ';
		global[i+5][j+2] = ' ';
		global[i+5][j+3] = ' ';

		global[i][j] = '0';
		global[i][j+1] = '0';
		global[i+1][j] = '0';
		global[i+1][j+1] = '0';
		global[i+2][j] = '0';
		global[i+2][j+1] = '0';
		global[i+3][j] = '0';
		global[i+3][j+1] = '0';
		global[i+2][j+4] = '0';
		global[i+2][j+5] = '0';
		global[i+3][j+4] = '0';
		global[i+3][j+5] = '0';

		current_position = 1;
	}
	else if(current_position == 1){

		if((j > largeur - 6) || (j < 0 )){

			return -2;
		}

		if((i > hauteur - 6) || (i < 0 )){

			return -3;
		}

		if((
			global[i][j+2] == 'O')||(
			global[i][j+3] == 'O')||(
			global[i][j+4] == 'O')||(
			global[i][j+5] == 'O')||(
			global[i+1][j+2] == 'O')||(
			global[i+1][j+3] == 'O')||(
			global[i+1][j+4] == 'O')||(
			global[i+1][j+5] == 'O')||(
			global[i+4][j+2] == 'O')||(
			global[i+4][j+3] == 'O')||(
			global[i+5][j+2] == 'O')||(
			global[i+5][j+3] == 'O')
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';
		global[i+2][j] = ' ';
		global[i+2][j+1] = ' ';
		global[i+3][j] = ' ';
		global[i+3][j+1] = ' ';
		global[i+2][j+4] = ' ';
		global[i+2][j+5] = ' ';
		global[i+3][j+4] = ' ';
		global[i+3][j+5] = ' ';

		global[i][j+2] = '0';
		global[i][j+3] = '0';
		global[i][j+4] = '0';
		global[i][j+5] = '0';
		global[i+1][j+2] = '0';
		global[i+1][j+3] = '0';
		global[i+1][j+4] = '0';
		global[i+1][j+5] = '0';
		global[i+4][j+2] = '0';
		global[i+4][j+3] = '0';
		global[i+5][j+2] = '0';
		global[i+5][j+3] = '0';

		current_position = 2;
	}
	else if(current_position == 2){

		if((j > largeur - 4) || (j <  2)){

			return -2;
		}

		if((i > hauteur - 6) || (i <  0)){

			return -3;
		}

		if((
			global[i+2][j-1] == 'O')||(
			global[i+2][j-2] == 'O')||(
			global[i+3][j-1] == 'O')||(
			global[i+3][j-2] == 'O')||(
			global[i+2][j+2] == 'O')||(
			global[i+2][j+3] == 'O')||(
			global[i+3][j+2] == 'O')||(
			global[i+3][j+3] == 'O')||(
			global[i+4][j+2] == 'O')||(
			global[i+4][j+3] == 'O')||(
			global[i+5][j+2] == 'O')||(
			global[i+5][j+3] == 'O')
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i][j+2] = ' ';
		global[i][j+3] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';
		global[i+1][j+2] = ' ';
		global[i+1][j+3] = ' ';
		global[i+4][j] = ' ';
		global[i+4][j+1] = ' ';
		global[i+5][j] = ' ';
		global[i+5][j+1] = ' ';

		global[i+2][j-1] = '0';
		global[i+2][j-2] = '0';
		global[i+3][j-1] = '0';
		global[i+3][j-2] = '0';
		global[i+2][j+2] = '0';
		global[i+2][j+3] = '0';
		global[i+3][j+2] = '0';
		global[i+3][j+3] = '0';
		global[i+4][j+2] = '0';
		global[i+4][j+3] = '0';
		global[i+5][j+2] = '0';
		global[i+5][j+3] = '0';
		
		current_position = 3;
	}
	else if(current_position == 3){

		if((j > largeur - 6) || (j < 0 )){

			return -2;
		}

		if((i > hauteur - 4) || (i < 2 )){

			return -3;
		}

		if((
			global[i-1][j+2] == 'O')||(
			global[i-1][j+3] == 'O')||(
			global[i-2][j+2] == 'O')||(
			global[i-2][j+3] == 'O')||(
			global[i+2][j+2] == 'O')||(
			global[i+2][j+3] == 'O')||(
			global[i+3][j+2] == 'O')||(
			global[i+3][j+3] == 'O')||(
			global[i+2][j] == 'O')||(
			global[i+2][j+1] == 'O')||(
			global[i+3][j] == 'O')||(
			global[i+3][j+1] == 'O')
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i][j+4] = ' ';
		global[i][j+5] = ' ';

		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';
		global[i+1][j+4] = ' ';
		global[i+1][j+5] = ' ';

		global[i+2][j+4] = ' ';
		global[i+2][j+5] = ' ';
		global[i+3][j+4] = ' ';
		global[i+3][j+5] = ' ';

		global[i-1][j+2] = '0';
		global[i-1][j+3] = '0';
		global[i-2][j+2] = '0';
		global[i-2][j+3] = '0';

		global[i+2][j+2] = '0';
		global[i+2][j+3] = '0';
		global[i+3][j+2] = '0';
		global[i+3][j+3] = '0';

		global[i+2][j] = '0';
		global[i+2][j+1] = '0';
		global[i+3][j] = '0';
		global[i+3][j+1] = '0';
		
		current_position = 0;
	}

	return 0;

}

int pivoterLDroit(){
	int i = 0;
	int j = 0;

	while(i < hauteur && (findZInLine(i) == -1) ){
		i++;
	}

	while(j < largeur && (findZInCol(j) == -1) ){
		j++;
	}

	if (current_position == 0){  // L


		if((j > largeur - 4) || (j < 2 )){

			return -2;
		}

		if((i > hauteur - 6) || (i < 0 )){

			return -3;
		}

		if((
			global[i+2][j-1] == 'O')||(
			global[i+2][j-2] == 'O')||(
			global[i+3][j-1] == 'O')||(
			global[i+3][j-2] == 'O')||(
			global[i+2][j+2] == 'O')||(
			global[i+2][j+3] == 'O')||(
			global[i+3][j+2] == 'O')||(
			global[i+3][j+3] == 'O')||(
			global[i+4][j-1] == 'O')||(
			global[i+4][j-2] == 'O')||(
			global[i+5][j-1] == 'O')||(
			global[i+5][j-2] == 'O')
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';

		global[i+4][j] = ' ';
		global[i+4][j+1] = ' ';
		global[i+5][j] = ' ';
		global[i+5][j+1] = ' ';

		global[i+4][j+2] = ' ';
		global[i+4][j+3] = ' ';
		global[i+5][j+2] = ' ';
		global[i+5][j+3] = ' ';

		global[i+2][j-1] = '0';
		global[i+2][j-2] = '0';
		global[i+3][j-1] = '0';
		global[i+3][j-2] = '0';

		global[i+2][j+2] = '0';
		global[i+2][j+3] = '0';
		global[i+3][j+2] = '0';
		global[i+3][j+3] = '0';

		global[i+4][j-1] = '0';
		global[i+4][j-2] = '0';
		global[i+5][j-1] = '0';
		global[i+5][j-2] = '0';

		current_position = 1;

	}
	else if (current_position == 1){ 


		if((j > largeur - 6) || (j < 0 )){

			return -2;
		}

		if((i > hauteur - 4) || (i < 2 )){

			return -3;
		}

		if((
			global[i-2][j+2] == 'O')||(
			global[i-2][j+3] == 'O')||(
			global[i-1][j+2] == 'O')||(
			global[i-1][j+3] == 'O')||(
			global[i+2][j+2] == 'O')||(
			global[i+2][j+3] == 'O')||(
			global[i+3][j+2] == 'O')||(
			global[i+3][j+3] == 'O')||(
			global[i-2][j] == 'O')||(
			global[i-2][j+1] == 'O')||(
			global[i-1][j] == 'O')||(
			global[i-1][j+1] == 'O')
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';

		global[i+2][j] = ' ';
		global[i+2][j+1] = ' ';
		global[i+3][j] = ' ';
		global[i+3][j+1] = ' ';

		global[i][j+4] = ' ';
		global[i][j+5] = ' ';
		global[i+1][j+4] = ' ';
		global[i+1][j+5] = ' ';

		global[i-2][j+2] = '0';
		global[i-2][j+3] = '0';
		global[i-1][j+2] = '0';
		global[i-1][j+3] = '0';

		global[i+2][j+2] = '0';
		global[i+2][j+3] = '0';
		global[i+3][j+2] = '0';
		global[i+3][j+3] = '0';

		global[i-2][j] = '0';
		global[i-2][j+1] = '0';
		global[i-1][j] = '0';
		global[i-1][j+1] = '0';

		current_position = 2;

	}
	else if (current_position == 2){ 


		if((j > largeur - 6) || (j < 0 )){

			return -2;
		}

		if((i > hauteur - 6) || (i < 0 )){

			return -3;
		}

		if((
			global[i+2][j] == 'O')||(
			global[i+2][j+1] == 'O')||(
			global[i+3][j] == 'O')||(
			global[i+3][j+1] == 'O')||(
			global[i+2][j+4] == 'O')||(
			global[i+2][j+5] == 'O')||(
			global[i+3][j+4] == 'O')||(
			global[i+3][j+5] == 'O')||(
			global[i][j+4] == 'O')||(
			global[i][j+5] == 'O')||(
			global[i+1][j+4] == 'O')||(
			global[i+1][j+5] == 'O')
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';

		global[i][j+2] = ' ';
		global[i][j+3] = ' ';
		global[i+1][j+2] = ' ';
		global[i+1][j+3] = ' ';

		global[i+4][j+2] = ' ';
		global[i+4][j+3] = ' ';
		global[i+5][j+2] = ' ';
		global[i+5][j+3] = ' ';

		global[i+2][j] = '0';
		global[i+2][j+1] = '0';
		global[i+3][j] = '0';
		global[i+3][j+1] = '0';

		global[i+2][j+4] = '0';
		global[i+2][j+5] = '0';
		global[i+3][j+4] = '0';
		global[i+3][j+5] = '0';

		global[i][j+4] = '0';
		global[i][j+5] = '0';
		global[i+1][j+4] = '0';
		global[i+1][j+5] = '0';

		current_position = 3;

	}

	else if (current_position == 3){ 


		if((j > largeur - 6) || (j < 0 )){

			return -2;
		}

		if((i > hauteur - 6) || (i < 0 )){

			return -3;
		}

		if((
			global[i][j+2] == 'O')||(
			global[i][j+3] == 'O')||(
			global[i+1][j+2] == 'O')||(
			global[i+1][j+3] == 'O')||(
			global[i+4][j+2] == 'O')||(
			global[i+4][j+3] == 'O')||(
			global[i+5][j+2] == 'O')||(
			global[i+5][j+3] == 'O')||(
			global[i+4][j+4] == 'O')||(
			global[i+4][j+5] == 'O')||(
			global[i+5][j+4] == 'O')||(
			global[i+5][j+5] == 'O')
		)
		{
			return -4;
		}

		global[i][j+4] = ' ';
		global[i][j+5] = ' ';
		global[i+1][j+4] = ' ';
		global[i+1][j+5] = ' ';

		global[i+2][j] = ' ';
		global[i+2][j+1] = ' ';
		global[i+3][j] = ' ';
		global[i+3][j+1] = ' ';

		global[i+2][j+4] = ' ';
		global[i+2][j+5] = ' ';
		global[i+3][j+4] = ' ';
		global[i+3][j+5] = ' ';

		global[i][j+2] = '0';
		global[i][j+3] = '0';
		global[i+1][j+2] = '0';
		global[i+1][j+3] = '0';

		global[i+4][j+2] = '0';
		global[i+4][j+3] = '0';
		global[i+5][j+2] = '0';
		global[i+5][j+3] = '0';

		global[i+4][j+4] = '0';
		global[i+4][j+5] = '0';
		global[i+5][j+4] = '0';
		global[i+5][j+5] = '0';

		current_position = 0;

	}

	return 0;

}

int pivoterBarre(){
	int i = 0;
	int j = 0;

	while(i < hauteur && (findZInLine(i) == -1) ){
		i++;
	}

	while(j < largeur && (findZInCol(j) == -1) ){
		j++;
	}

	// i, j est maintenant le point haut gauche de la barre
	
	if (current_position == 0){ // | -> _

		if((j > largeur - 4) || (j < 4 )){

			return -2;
		}

		if((i > hauteur - 8) || (i < 0 )){

			return -3;
		}

		if((
			global[i+4][j-4] == 'O' ) || (
			global[i+5][j-4] == 'O' ) || (
			global[i+4][j-3] == 'O' ) || (
			global[i+5][j-3] == 'O' ) || (
			global[i+4][j-2] == 'O' ) || (
			global[i+5][j-2] == 'O' ) || (
			global[i+4][j-1] == 'O' ) || (
			global[i+5][j-1] == 'O' ) || (
			global[i+4][j] == 'O' ) || (
			global[i+5][j] == 'O' ) || (
			global[i+4][j+1] == 'O' ) || (
			global[i+5][j+1] == 'O' ) || (
			global[i+4][j+2] == 'O' ) || (
			global[i+5][j+2] == 'O' ) || (
			global[i+4][j+3] == 'O' ) || (
			global[i+5][j+3] == 'O' ))
		{
			return -4;
		}

		// on supprime la barre
		global[i][j] = ' ';
		global[i+1][j] = ' ';
		global[i+2][j] = ' ';
		global[i+3][j] = ' ';
		global[i+4][j] = ' ';
		global[i+5][j] = ' ';
		global[i+6][j] = ' ';
		global[i+7][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j+1] = ' ';
		global[i+2][j+1] = ' ';
		global[i+3][j+1] = ' ';
		global[i+4][j+1] = ' ';
		global[i+5][j+1] = ' ';
		global[i+6][j+1] = ' ';
		global[i+7][j+1] = ' ';

		// on ajoute la "nouvelle" horizontale
		global[i+4][j-4] = '0';
		global[i+5][j-4] = '0';
		global[i+4][j-3] = '0';
		global[i+5][j-3] = '0';
		global[i+4][j-2] = '0';
		global[i+5][j-2] = '0';
		global[i+4][j-1] = '0';
		global[i+5][j-1] = '0';
		global[i+4][j] = '0';
		global[i+5][j] = '0';
		global[i+4][j+1] = '0';
		global[i+5][j+1] = '0';
		global[i+4][j+2] = '0';
		global[i+5][j+2] = '0';
		global[i+4][j+3] = '0';
		global[i+5][j+3] = '0';

		current_position = 1;

		return 1;

	}
	else{  // _ -> |

		if((j < 0) || (j > largeur - 8 )){

			return -2;
		}

		if((i < 4 ) || (i > hauteur - 4 )){

			return -3;
		}

		if((
			global[i][j+4] == 'O' )||(
			global[i][j+5] == 'O' )||(
			global[i+1][j+4] == 'O' )||(
			global[i+1][j+5] == 'O' )||(
			global[i+2][j+4] == 'O' )||(
			global[i+2][j+5] == 'O' )||(
			global[i+3][j+4] == 'O' )||(
			global[i+3][j+5] == 'O' )||(
			global[i-1][j+4] == 'O' )||(
			global[i-1][j+5] == 'O' )||(
			global[i-2][j+4] == 'O' )||(
			global[i-2][j+5] == 'O' )||(
			global[i-3][j+4] == 'O' )||(
			global[i-3][j+5] == 'O' )||(
			global[i-4][j+4] == 'O' )||(
			global[i-4][j+5] == 'O' )
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i][j+2] = ' ';
		global[i][j+3] = ' ';
		global[i][j+4] = ' ';
		global[i][j+5] = ' ';
		global[i][j+6] = ' ';
		global[i][j+7] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';
		global[i+1][j+2] = ' ';
		global[i+1][j+3] = ' ';
		global[i+1][j+4] = ' ';
		global[i+1][j+5] = ' ';
		global[i+1][j+6] = ' ';
		global[i+1][j+7] = ' ';

		global[i][j+4] = '0';
		global[i][j+5] = '0';
		global[i+1][j+4] = '0';
		global[i+1][j+5] = '0';
		global[i+2][j+4] = '0';
		global[i+2][j+5] = '0';
		global[i+3][j+4] = '0';
		global[i+3][j+5] = '0';
		global[i-1][j+4] = '0';
		global[i-1][j+5] = '0';
		global[i-2][j+4] = '0';
		global[i-2][j+5] = '0';
		global[i-3][j+4] = '0';
		global[i-3][j+5] = '0';
		global[i-4][j+4] = '0';
		global[i-4][j+5] = '0';

		
		current_position = 0;
	}

	return 0;
}

int pivoterQuatre(){
	int i = 0;
	int j = 0;

	while(i < hauteur && (findZInLine(i) == -1) ){
		i++;
	}

	while(j < largeur && (findZInCol(j) == -1) ){
		j++;
	}

	if(current_position == 0){

		if((i < 0 ) || (i > hauteur - 6 )){

			return -3;
		}

		if((j < 2) || (j > largeur - 4 )){

			return -2;
		}

		if((
			global[i+4][j-2] == 'O' )||(
			global[i+4][j-1] == 'O' )||(
			global[i+5][j-2] == 'O' )||(
			global[i+5][j-1] == 'O' )||(
			global[i+4][j] == 'O' )||(
			global[i+4][j+1] == 'O' )||(
			global[i+5][j] == 'O' )||(
			global[i+5][j+1] == 'O' )
		)
		{
			return -4;
		}

		global[i+4][j+2] = ' ';
		global[i+4][j+3] = ' ';
		global[i+5][j+2] = ' ';
		global[i+5][j+3] = ' ';

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';

		global[i+4][j-2] = '0';
		global[i+4][j-1] = '0';
		global[i+5][j-2] = '0';
		global[i+5][j-1] = '0';

		global[i+4][j] = '0';
		global[i+4][j+1] = '0';
		global[i+5][j] = '0';
		global[i+5][j+1] = '0';

		current_position = 1;
	}
	else{

		if((i < 2 ) || (i > hauteur - 4 )){

			return -3;
		}

		if((j < 0) || (j > largeur - 6 )){

			return -2;
		}

		if((
			global[i-1][j+2] == 'O' )||(
			global[i-1][j+3] == 'O' )||(
			global[i-2][j+2] == 'O' )||(
			global[i-2][j+3] == 'O' )||(

			global[i+2][j+4] == 'O' )||(
			global[i+2][j+5] == 'O' )||(
			global[i+3][j+4] == 'O' )||(
			global[i+3][j+5] == 'O' )
		)
		{
			return -4;
		}

		global[i+2][j] = ' ';
		global[i+2][j+1] = ' ';
		global[i+3][j] = ' ';
		global[i+3][j+1] = ' ';

		global[i+2][j+2] = ' ';
		global[i+2][j+3] = ' ';
		global[i+3][j+2] = ' ';
		global[i+3][j+3] = ' ';

		global[i-1][j+2] = '0';
		global[i-1][j+3] = '0';
		global[i-2][j+2] = '0';
		global[i-2][j+3] = '0';

		global[i+2][j+4] = '0';
		global[i+2][j+5] = '0';
		global[i+3][j+4] = '0';
		global[i+3][j+5] = '0';

		current_position = 0;
	}

	return 1;
}

int pivoterQInverse(){
	int i = 0;
	int j = 0;

	while(i < hauteur && (findZInLine(i) == -1) ){
		i++;
	}

	while(j < largeur && (findZInCol(j) == -1) ){
		j++;
	}

	if(current_position == 0){

		if((i < 0 ) || (i > hauteur - 6 )){

			return -3;
		}

		if((j < 2) || (j > largeur - 4 )){

			return -2;
		}

		if((
			global[i+2][j-1] == 'O' )||(
			global[i+2][j-2] == 'O' )||(
			global[i+3][j-1] == 'O' )||(
			global[i+3][j-2] == 'O' )||(
			global[i+4][j+2] == 'O' )||(
			global[i+4][j+3] == 'O' )||(
			global[i+5][j+2] == 'O' )||(
			global[i+5][j+3] == 'O' )
		)
		{
			return -4;
		}

		global[i][j+2] = ' ';
		global[i][j+3] = ' ';
		global[i+1][j+2] = ' ';
		global[i+1][j+3] = ' ';

		global[i+2][j+2] = ' ';
		global[i+2][j+3] = ' ';
		global[i+3][j+2] = ' ';
		global[i+3][j+3] = ' ';

		global[i+2][j-1] = '0';
		global[i+2][j-2] = '0';
		global[i+3][j-1] = '0';
		global[i+3][j-2] = '0';

		global[i+4][j+2] = '0';
		global[i+4][j+3] = '0';
		global[i+5][j+2] = '0';
		global[i+5][j+3] = '0';

		current_position = 1;
	}
	else{

		if((i < 2 ) || (i > hauteur - 4 )){

			return -3;
		}

		if((j < 0) || (j > largeur - 6 )){

			return -2;
		}

		if((
			global[i-2][j+4] == 'O' )||(
			global[i-2][j+5] == 'O' )||(
			global[i-1][j+4] == 'O' )||(
			global[i-1][j+5] == 'O' )||(
			global[i][j+4] == 'O' )||(
			global[i][j+5] == 'O' )||(
			global[i+1][j+4] == 'O' )||(
			global[i+1][j+5] == 'O' )
		)
		{
			return -4;
		}

		global[i][j] = ' ';
		global[i][j+1] = ' ';
		global[i+1][j] = ' ';
		global[i+1][j+1] = ' ';

		global[i+2][j+4] = ' ';
		global[i+2][j+5] = ' ';
		global[i+3][j+4] = ' ';
		global[i+3][j+5] = ' ';

		global[i-2][j+4] = '0';
		global[i-2][j+5] = '0';
		global[i-1][j+4] = '0';
		global[i-1][j+5] = '0';

		global[i][j+4] = '0';
		global[i][j+5] = '0';
		global[i+1][j+4] = '0';
		global[i+1][j+5] = '0';

		current_position = 0;
	}

	return 1;
}

void pivoter(){
	switch (current_piece){
		case CARRE:
			// rien a faire...
		break;
		case BARRE:
			pivoterBarre();
		break;
		case T:
			pivoterT();
		break;
		case LGAUCHE:
			pivoterLGauche();
		break;
		case LDROIT:
			pivoterLDroit();
		break;
		case QUATRE:
			pivoterQuatre();
		break;
		case QINVERSE:
			pivoterQInverse();
		break;
		default:
			Assert(0);
		break;
	}
	hasChanged = 1;
}

void PutIntInAsciiArt(int i){
	if (i < 10){
		PutChar(' ');
		PutInt(i);
		PutChar(' ');
	}
	else if (i < 100){
		PutInt(i);
		PutChar(' ');
	}
	else{
		PutInt(i);
	}
}


int popCarre(){
	int i;

	for (i = 0; i<2*coeff; i++){
		if (global[i][milieu-1] == 'O' || 
			global[i][milieu] == 'O' || 
			global[i][milieu+1] == 'O' || 
			global[i][milieu+2] == 'O')
		{
			return 0;
		}
	}

	for (i = 0; i<2*coeff; i++){
		global[i][milieu-1] = '0';
		global[i][milieu] = '0';
		global[i][milieu+1] = '0';
		global[i][milieu+2] = '0';
	}
	current_piece = CARRE;
	current_position = 0;
	hasChanged = 1;
	return 1;
}

int popBarre(){
	int i;

	for (i = 0; i<4*coeff; i++){
		if (global[i][milieu] == 'O' ||
		global[i][milieu-1] == 'O')
		{
			return 0;
		}
	}

	for (i = 0; i<4*coeff; i++){
		global[i][milieu] = '0';
		global[i][milieu-1] = '0';
	}

	current_piece = BARRE;
	current_position = 0;
	hasChanged = 1;
	return 1;
}

int popQuatre(){
	int i;

	for (i = 0; i<2*coeff; i++){
		if (global[i][milieu] == 'O' ||
		global[i][milieu-1] == 'O')
		{
			return 0;
		}
	}

	for (i = 1*coeff; i < 3*coeff; i++){
		if (global[i][milieu+1] == 'O' ||
		global[i][milieu+2] == 'O')
		{
			return 0;
		}
	}

	for(i = 0; i < 2*coeff; i++){
		global[i][milieu] = '0';
		global[i][milieu-1] = '0';
	}
	for(i = 1*coeff; i < 3*coeff; i++){
		global[i][milieu+2] = '0';
		global[i][milieu+1] = '0';
	}
	

	current_piece = QUATRE;
	current_position = 0;
	hasChanged = 1;
	return 1;
}

int popQInverse(){
	int i;

	for (i = 0; i<2*coeff; i++){
		if (global[i][milieu+1] == 'O' ||
		global[i][milieu+2] == 'O')
		{
			return 0;
		}
	}

	for (i = 1*coeff; i < 3*coeff; i++){
		if (global[i][milieu] == 'O' ||
		global[i][milieu-1] == 'O')
		{
			return 0;
		}
	}

	for(i = 0; i < 2*coeff; i++){
		global[i][milieu+1] = '0';
		global[i][milieu+2] = '0';
	}
	for(i = 1*coeff; i < 3*coeff; i++){
		global[i][milieu] = '0';
		global[i][milieu-1] = '0';
	}
	

	current_piece = QINVERSE;
	current_position = 0;
	hasChanged = 1;
	return 1;
}

int popLDroit(){
	int i;

	for (i = 0; i<3*coeff; i++){
		if(global[i][milieu] == 'O' || global[i][milieu-1] == 'O'){
			return 0;
		}
		global[i][milieu] = '0';
		global[i][milieu-1] = '0';
	}
	i--;
	int k;
	for (k = i-coeff; i>k; i--){
		if(global[i][milieu+1] == 'O' || global[i][milieu+2] == 'O'){
			return 0;
		}
		global[i][milieu+1] = '0';
		global[i][milieu+2] = '0';
	}
	current_piece = LDROIT;
	current_position = 0;
	hasChanged = 1;
	return 1;
}

int popLGauche(){
	int i;

	for (i = 0; i<3*coeff; i++){
		if(global[i][milieu] == 'O' || global[i][milieu-1] == 'O'){
			return 0;
		}
		global[i][milieu] = '0';
		global[i][milieu-1] = '0';
	}
	i--;
	int k;
	for (k = i-coeff; i>k; i--){
		if(global[i][milieu-2] == 'O' || global[i][milieu-3] == 'O'){
			return 0;
		}
		global[i][milieu-2] = '0';
		global[i][milieu-3] = '0';
	}
	current_piece = LGAUCHE;
	current_position = 0;
	hasChanged = 1;
	return 1;
}

int popT(){
	int i;

	for (i = 0; i<2*coeff; i++){
		if(global[i][milieu] == 'O' || global[i][milieu-1] == 'O'){
			return 0;
		}
		global[i][milieu] = '0';
		global[i][milieu-1] = '0';
	}
	if((
		global[0][milieu-2] == 'O' )||(
		global[0][milieu-3] == 'O' )||(
		global[1][milieu-2] == 'O' )||(
		global[1][milieu-3] == 'O' )||(
		global[0][milieu+2] == 'O' )||(
		global[0][milieu+1] == 'O' )||(
		global[1][milieu+2] == 'O' )||(
		global[1][milieu+1] == 'O' ))
	{
		return 0;
	}
	global[0][milieu-2] = '0';
	global[0][milieu-3] = '0';
	global[1][milieu-2] = '0';
	global[1][milieu-3] = '0';
	global[0][milieu+2] = '0';
	global[0][milieu+1] = '0';
	global[1][milieu+2] = '0';
	global[1][milieu+1] = '0';
	current_piece = T;
	current_position = 0;
	hasChanged = 1;
	return 1;
}

void clearscr(){
	PutString(clrscrArray);
}


// 0 = arret demande, 1 = a descendre, 2 = rien a faire
int checkOKDescendreLigne(){
	//PutString("checkOKDescendreLigne\n");
	int j;
	int linesearch;

	for(linesearch = hauteur-1 ; linesearch >= 0; linesearch--){
		for (j = 0; j<largeur; j++){
			if(global[linesearch][j] == '0' && linesearch == hauteur-1){
				return 0;
			}
			if(global[linesearch][j] == '0'){
				if(linesearch+1 <= hauteur && global[linesearch+1][j] == 'O'){
					return 0;
				}
			}
		}
	}

	return 1;

}

int checkOKGoLeft(){
	//PutString("checkOKGoLeft\n");
	int i;
	int j;
	for(i = 0; i<hauteur; i++){
		j = 1;
		while(j < largeur && global[i][j] != '0' && global[i][j-1] != '0' ){
			j++;
		}
		if(j <= 1){
			return 0;
		}
		if(j < largeur && global[i][j-1] != ' ' && global[i][j-2] != ' '){
			return 0;
		}
	}
	return 1;
}

void goLeft(){
	//PutString("goLeft\n");
	int ok = checkOKGoLeft();
	int j;
	int i;
	if(ok){
		for(j = 1; j < largeur; j++){
			for(i = 0; i< hauteur; i++){
				if(global[i][j] == '0'){
					global[i][j] = ' ';
					global[i][j-1] = '0';
					hasChanged = 1;
				}
			}
		}
		for(j = 1; j < largeur; j++){
			for(i = 0; i< hauteur; i++){
				if(global[i][j] == '0'){
					global[i][j] = ' ';
					global[i][j-1] = '0';
					hasChanged = 1;
				}
			}
		}
	}
}

int checkOKGoRight(){
	//PutString("checkOKGoRight\n");
	int i;
	int j;
	for(i = 0; i<hauteur; i++){
		j = largeur-1;
		while(j >= 0 && global[i][j] != '0'){
			j--;
		}
		if(j == largeur-1){
			return 0;
		}
		if(j >= 0 && global[i][j+1] != ' '){
			return 0;
		}
	}
	return 1;
}

void goRight(){
	//PutString("goRight\n");
	int ok = checkOKGoRight();
	int j = largeur-1;
	int i;
	if(ok){
		for(j = largeur-1; j >= 0 ; j--){
			for(i = 0; i< hauteur; i++){
				if(global[i][j] == '0'){
					global[i][j] = ' ';
					global[i][j+1] = '0';
					hasChanged = 1;
				}
			}
		}
		for(j = largeur-1; j >= 0 ; j--){
			for(i = 0; i< hauteur; i++){
				if(global[i][j] == '0'){
					global[i][j] = ' ';
					global[i][j+1] = '0';
					hasChanged = 1;
				}
			}
		}
	}
}


void fixLigne(int ligne){
	////PutString("fixLigne\n");
	int j;
	for(j = 0; j<largeur; j++){
		if(global[ligne][j] == '0'){
			global[ligne][j] = 'O';
			hasChanged = 1;
		}
	}
}

void descendreLigne(int ligne){
	////PutString("descendreLigne\n");
	int j;
	for(j = 0; j<largeur; j++){
		if(global[ligne][j] == '0'){
			global[ligne][j] = ' ';
			global[ligne+1][j] = '0';
			hasChanged = 1;
		}
	}
}

void AffSmallNext(){
	int i = numPiece + 1;
	if(i >= nbPieceList){
		numPiece = 0;
	}
	switch(PieceList[i]){
		case CARRE:
			PutString("█          ║\n");
		break;
		case LGAUCHE:
			PutString("┘          ║\n");
		break;
		case LDROIT:
			PutString("└          ║\n");
		break;
		case BARRE:
			PutString("│          ║\n");
		break;
		case T:
			PutString("┬          ║\n");
		break;
		case QUATRE:
			PutString("└┐         ║\n");
		break;
		case QINVERSE:
			PutString("┌┘         ║\n");
		break;
		default:
			Assert(0);
		break;
	}
}

void printGlobal(){
	////PutString("printGlobal\n");
	int i;
	clearscr();
	PutString(" ╔══════════════════════╗");
	PutString(" ╔══════════════════════╗\n");
	for (i = 0; i < hauteur; i++ ){
		PutString(" ║ ");
		PutString(global[i]);
		switch(i){
			case hauteur-1:
				PutString(" ║ ║ Score : ");
				PutIntInAsciiArt(score/2);
				if(previousscore < score){
					PutString(" ⊂(▀¯▀⊂)  ║\n");
					previousscore = score;
				}
				else{
					PutString("          ║\n");
				}
				
			break;
			case hauteur-2:
				PutString(" ║ ║ Suivant : ");
				AffSmallNext();
			break;
			case hauteur-4:
				PutString(" ║ ║ 2+ENTER : Pivoter    ║\n");
			break;
			case hauteur-5:
				PutString(" ║ ║ 3+ENTER : Droite     ║\n");
			break;
			case hauteur-6:
				PutString(" ║ ║ 1+ENTER : Gauche     ║\n");
			break;
			case hauteur-7:
				PutString(" ║ ║ CONTROLES:           ║\n");
			break;
			case hauteur-8:
				PutString(" ║ ╔══════════════════════╗\n");
			break;
			case 0:
				PutString(" ║ ║     ┌─┐┌┬┐┌─┐┬─┐     ║\n");
			break;
			case 1:
				PutString(" ║ ║     └─┐ │ ├─┤├┬┘     ║\n");
			break;
			case 2:
				PutString(" ║ ║     └─┘ ┴ ┴ ┴┴└─     ║\n");
			break;
			case 3:
				PutString(" ║ ║   ┌┬┐┌─┐┌┬┐┬─┐┬┌─┐   ║\n");
			break;
			case 4:
				PutString(" ║ ║    │ ├┤  │ ├┬┘│└─┐   ║\n");
			break;
			case 5:
				PutString(" ║ ║    ┴ └─┘ ┴ ┴└─┴└─┘   ║\n");
			break;
			case 6:
				PutString(" ║ ║      For NachOS      ║\n");
			break;
			case 7:
				PutString(" ║ ╚══════════════════════╝\n");
			break;
			case hauteur-3:
				PutString(" ║ ╠══════════════════════╣\n");
			break;
			default:
			PutString(" ║\n");
			break;
		}
	}
	PutString(" ╚══════════════════════╝");
	PutString(" ╚══════════════════════╝\n>");
	hasChanged = 0;
}

void supprimeLigne(){
	int i;
	int j;
	int itemp;
	int okToDelete;
	for(i = hauteur-1; i>=0; i--){
		okToDelete = 1;
		for(j = 0; j< largeur; j++){
			if(global[i][j] == ' ' || global[i][j] == '0'){
				okToDelete = 0;
			}
		}

		if(okToDelete){
			for(itemp = i; itemp > 1; itemp--){
				for(j = 0; j< largeur; j++){
					global[itemp][j] = global[itemp-1][j];
				}
			}
			hasChanged = 1;
			score++;
		}
	}
}

void descendreGlobal(int diff){
	////PutString("descendreGlobal\n");
	int current_ligne;
	int i;
	int check = 0;
	for(i = 0; i<hauteur; i++){
		UserSemP(sem_action);
		if(askedAction[0] == 1){
			goLeft();
			askedAction[0] = 0;
		}
		if(askedAction[0] == 2){
			pivoter();
			askedAction[0] = 0;
		}
		if(askedAction[0] == 3){
			goRight();
			askedAction[0] = 0;
		}
		if(askedAction[1] == 1){
			goLeft();
			askedAction[1] = 0;
		}
		if(askedAction[1] == 2){
			pivoter();
			askedAction[1] = 0;
		}
		if(askedAction[1] == 3){
			goRight();
			askedAction[1] = 0;
		}
		UserSemV(sem_action);
		current_ligne = hauteur-1;
		check = checkOKDescendreLigne();
		while(current_ligne >= 0){
			if (check){
				descendreLigne(current_ligne);
			}
			else{
				fixLigne(current_ligne);
			}
			current_ligne--;
		}
		if(i % 2 == 0){
			supprimeLigne();
		}
		if(hasChanged){
			CustomUsleep(diff * 12);
			printGlobal();
		}
	}
}

void controlThread(void * v){
	int ch;
	UserSemP(sem_run);
	while(goKillYourself == 0){
		ch = -1;
		ch = GetCharInt(); // on évite la convertion, much performance 49 = 1; 50 = 2; 51 = 3
		UserSemP(sem_action);
		if(askedAction[0] == 0){
			if(ch == 49){
				askedAction[0] = 1;
			}
			if(ch == 50){
				askedAction[0] = 2;
			}
			if(ch == 51){
				askedAction[0] = 3;
			}
		}
		else{
			if(ch == 49){
				askedAction[1] = 1;
			}
			if(ch == 50){
				askedAction[1] = 2;
			}
			if(ch == 51){
				askedAction[1] = 3;
			}
		}

		UserSemV(sem_action);
	}
}

int popNext(){
	numPiece++;
	if(numPiece >= nbPieceList){
		numPiece = 0;
	}
	switch(PieceList[numPiece]){
		case CARRE:
			return popCarre();
		break;
		case LGAUCHE:
			return popLGauche();
		break;
		case LDROIT:
			return popLDroit();
		break;
		case BARRE:
			return popBarre();
		break;
		case T:
			return popT();
		break;
		case QUATRE:
			return popQuatre();
		break;
		case QINVERSE:
			return popQInverse();
		break;
		default:
			Assert(0);
			return 0;
		break;
	}
}

void getNomHighScore(char buffer[16]){
	buffer[0] = 'T';
	buffer[1] = 'h';
	buffer[2] = 'e';
	buffer[3] = 'L';
	buffer[4] = 'e';
	buffer[5] = 'g';
	buffer[6] = 'e';
	buffer[7] = 'n';
	buffer[8] = 'd';
	buffer[9] = '2';
	buffer[10] = '7';
	buffer[11] = ' ';
	buffer[12] = '\0';

}

void gameOver(){
	score = score/2;
	int i;
	char nomHiSc[16];
	for(i = 0; i<12; i++){
		nomHiSc[i] = ' ';
	}
	for(i = 12; i<16; i++){
		nomHiSc[i] = '\0';
	}
	getNomHighScore(nomHiSc);
	clearscr();
	PutString("         ::::::::           :::          :::   :::       :::::::::: \n");
	PutString("       :+:    :+:        :+: :+:       :+:+: :+:+:      :+:         \n");
	PutString("      +:+              +:+   +:+     +:+ +:+:+ +:+     +:+          \n");
	PutString("     :#:             +#++:++#++:    +#+  +:+  +#+     +#++:++#      \n");
	PutString("    +#+   +#+#      +#+     +#+    +#+       +#+     +#+            \n");
	PutString("   #+#    #+#      #+#     #+#    #+#       #+#     #+#             \n");
	PutString("   ########       ###     ###    ###       ###     ##########       \n");
	PutString("\n\n\n");
	PutString("         ::::::::    :::     :::       ::::::::::       :::::::::   \n");
	PutString("       :+:    :+:   :+:     :+:       :+:              :+:    :+:   \n");
	PutString("      +:+    +:+   +:+     +:+       +:+              +:+    +:+    \n");
	PutString("     +#+    +:+   +#+     +:+       +#++:++#         +#++:++#:      \n");
	PutString("    +#+    +#+    +#+   +#+        +#+              +#+    +#+      \n");
	PutString("   #+#    #+#     #+#+#+#         #+#              #+#    #+#       \n");
	PutString("   ########        ###           ##########       ###    ###        \n");
	PutString("\n\n\n");
	PutString(" ╔═════════════════════");
	PutString("═════════════════════");
	PutString("═════════════════════╗\n");
	PutString(" ║                                                               ║");
	PutString("\n ║ Votre score : ");
	PutIntInAsciiArt(score);
	PutString("                                             ║");
	PutString("\n ║                                                               ║");
	PutString("\n ╚═════════════════════");
	PutString("═════════════════════");
	PutString("═════════════════════╝\n");
}

void play(int diff){
	//int speed = (int)s;
	// on vérifie si on a perdu
	while(popNext()){
		printGlobal();
		descendreGlobal(diff);
	}
	PutString("Appuyer sur entrée pour continuer\n");
	goKillYourself = 1;
	gameOver();
	PutString("Appuyer sur entrée pour continuer\n");
}

void printMenu(){
	char ch;
	PutString("        :::   :::       ::::::::::       ::::    :::      :::    ::: \n");
	PutString("      :+:+: :+:+:      :+:              :+:+:   :+:      :+:    :+:  \n");
	PutString("    +:+ +:+:+ +:+     +:+              :+:+:+  +:+      +:+    +:+   \n");
	PutString("   +#+  +:+  +#+     +#++:++#         +#+ +:+ +#+      +#+    +:+    \n");
	PutString("  +#+       +#+     +#+              +#+  +#+#+#      +#+    +#+     \n");
	PutString(" #+#       #+#     #+#              #+#   #+#+#      #+#    #+#      \n");
	PutString("###       ###     ##########       ###    ####       ########        \n");
	PutString("\n\n\n");
	PutString(" ╔═════════════════════");
	PutString("═════════════════════");
	PutString("═════════════════════╗\n");
	PutString(" ║                                                               ║");
	PutString("\n ║ n : Lancer une partie (normale)                               ║");
	PutString("\n ║ d : Lancer une partie (difficile)                             ║");
	PutString("\n ║ q : quitter                                                   ║");
	PutString("\n ║                                                               ║");
	PutString("\n ║ CONTROLES (du jeu) :                                          ║");
	PutString("\n ║                                                               ║");
	PutString("\n ║ 1 + ENTER : Déplacer à gauche                                 ║");
	PutString("\n ║ 2 + ENTER : Pitover                                           ║");
	PutString("\n ║ 3 + ENTER : Déplacer à gauche                                 ║");
	PutString("\n ║                                                               ║\n");
	PutString(" ╚═════════════════════");
	PutString("═════════════════════");
	PutString("═════════════════════╝\n");
	PutString("\n>");
	ch = (char)GetCharInt();
	while(ch != 'n' && ch != 'd' && ch != 'a' && ch != 'q'){
		PutString("\n>");
		ch = (char)GetCharInt();
	}
	if(ch == 'n'){
		UserSemV(sem_run);
		play(2);
	}
	if(ch == 'd'){
		UserSemV(sem_run);
		play(1);
	}
	if(ch == 'a'){
		goKillYourself = 1;
		UserSemV(sem_run);
	}
	if(ch == 'q'){
		goKillYourself = 1;
		UserSemV(sem_run);
	}
}

void printLogo(){
	int speed = 2;
	clearscr();
	PutString("           _______..___________.     ___      .______        \n");
	PutString("          /       ||           |    /   \\     |   _  \\            \n");
	PutString("         |   (----``---|  |----`   /  ^  \\    |  |_)  |           \n");
	PutString("          \\   \\        |  |       /  /_\\  \\   |      /            \n");
	PutString("      .----)   |       |  |      /  _____  \\  |  |\\  \\----.       \n");
	PutString("      |_______/        |__|     /__/     \\__\\ | _| `._____|       \n");
	PutString("\n\n\n\n\n\n");
	PutString("\n\n\n\n\n\n\n");
	PutString("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	CustomUsleep(speed * 15);
	clearscr();
	PutString("           _______..___________.     ___      .______   \n");
	PutString("          /       ||           |    /   \\     |   _  \\            \n");
	PutString("         |   (----``---|  |----`   /  ^  \\    |  |_)  |           \n");
	PutString("          \\   \\        |  |       /  /_\\  \\   |      /            \n");
	PutString("      .----)   |       |  |      /  _____  \\  |  |\\  \\----.       \n");
	PutString("      |_______/        |__|     /__/     \\__\\ | _| `._____|       \n");
	PutString(".___________. _______ .___________..______       __       _______.\n");
	PutString("|           ||   ____||           ||   _  \\     |  |     /       |\n");
	PutString("`---|  |----`|  |__   `---|  |----`|  |_)  |    |  |    |   (----`\n");
	PutString("    |  |     |   __|      |  |     |      /     |  |     \\   \\    \n");
	PutString("    |  |     |  |____     |  |     |  |\\  \\----.|  | .----)   |   \n");
	PutString("    |__|     |_______|    |__|     | _| `._____||__| |_______/    \n");
	PutString("\n\n\n\n\n\n\n");
	PutString("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	CustomUsleep(speed * 25);
	clearscr();
	PutString("           _______..___________.     ___      .______   ( • _ • )     \n");
	PutString("          /       ||           |    /   \\     |   _  \\            \n");
	PutString("         |   (----``---|  |----`   /  ^  \\    |  |_)  |           \n");
	PutString("          \\   \\        |  |       /  /_\\  \\   |      /            \n");
	PutString("      .----)   |       |  |      /  _____  \\  |  |\\  \\----.       \n");
	PutString("      |_______/        |__|     /__/     \\__\\ | _| `._____|       \n");
	PutString(".___________. _______ .___________..______       __       _______.\n");
	PutString("|           ||   ____||           ||   _  \\     |  |     /       |\n");
	PutString("`---|  |----`|  |__   `---|  |----`|  |_)  |    |  |    |   (----`\n");
	PutString("    |  |     |   __|      |  |     |      /     |  |     \\   \\    \n");
	PutString("    |  |     |  |____     |  |     |  |\\  \\----.|  | .----)   |   \n");
	PutString("    |__|     |_______|    |__|     | _| `._____||__| |_______/    \n");
	PutString("\n\n\n\n\n\n\n");
	PutString("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	CustomUsleep(speed * 15);
	clearscr();
	PutString("           _______..___________.     ___      .______   ( • _ • ) > ⌐ ■ - ■\n");
	PutString("          /       ||           |    /   \\     |   _  \\            \n");
	PutString("         |   (----``---|  |----`   /  ^  \\    |  |_)  |           \n");
	PutString("          \\   \\        |  |       /  /_\\  \\   |      /            \n");
	PutString("      .----)   |       |  |      /  _____  \\  |  |\\  \\----.       \n");
	PutString("      |_______/        |__|     /__/     \\__\\ | _| `._____|       \n");
	PutString(".___________. _______ .___________..______       __       _______.\n");
	PutString("|           ||   ____||           ||   _  \\     |  |     /       |\n");
	PutString("`---|  |----`|  |__   `---|  |----`|  |_)  |    |  |    |   (----`\n");
	PutString("    |  |     |   __|      |  |     |      /     |  |     \\   \\    \n");
	PutString("    |  |     |  |____     |  |     |  |\\  \\----.|  | .----)   |   \n");
	PutString("    |__|     |_______|    |__|     | _| `._____||__| |_______/    \n");
	PutString("\n\n\n\n\n\n\n");
	PutString("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	CustomUsleep(speed * 15);
	clearscr();
	PutString("           _______..___________.     ___      .______   (⌐ ■ _■ )     \n");
	PutString("          /       ||           |    /   \\     |   _  \\            \n");
	PutString("         |   (----``---|  |----`   /  ^  \\    |  |_)  |           \n");
	PutString("          \\   \\        |  |       /  /_\\  \\   |      /            \n");
	PutString("      .----)   |       |  |      /  _____  \\  |  |\\  \\----.       \n");
	PutString("      |_______/        |__|     /__/     \\__\\ | _| `._____|       \n");
	PutString(".___________. _______ .___________..______       __       _______.\n");
	PutString("|           ||   ____||           ||   _  \\     |  |     /       |\n");
	PutString("`---|  |----`|  |__   `---|  |----`|  |_)  |    |  |    |   (----`\n");
	PutString("    |  |     |   __|      |  |     |      /     |  |     \\   \\    \n");
	PutString("    |  |     |  |____     |  |     |  |\\  \\----.|  | .----)   |   \n");
	PutString("    |__|     |_______|    |__|     | _| `._____||__| |_______/    \n");
	PutString("\n\n\n\n\n\n\n");
	PutString("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	CustomUsleep(speed * 20);
	clearscr();
	PutString("           _______..___________.     ___      .______   (⌐ ■ _■ )     \n");
	PutString("          /       ||           |    /   \\     |   _  \\            \n");
	PutString("         |   (----``---|  |----`   /  ^  \\    |  |_)  |           \n");
	PutString("          \\   \\        |  |       /  /_\\  \\   |      /            \n");
	PutString("      .----)   |       |  |      /  _____  \\  |  |\\  \\----.       \n");
	PutString("      |_______/        |__|     /__/     \\__\\ | _| `._____|       \n");
	PutString(".___________. _______ .___________..______       __       _______.\n");
	PutString("|           ||   ____||           ||   _  \\     |  |     /       |\n");
	PutString("`---|  |----`|  |__   `---|  |----`|  |_)  |    |  |    |   (----`\n");
	PutString("    |  |     |   __|      |  |     |      /     |  |     \\   \\    \n");
	PutString("    |  |     |  |____     |  |     |  |\\  \\----.|  | .----)   |   \n");
	PutString("    |__|     |_______|    |__|     | _| `._____||__| |_______/    \n");
	PutString("\n");
	PutString("  ═════════════════════");
	PutString("═════════════════════");
	PutString("═════════════════════\n");
	printMenu();
	
}

int
main()
{
	int i, j;
	for(j = 0; j<scrheight; j++){
		clrscrArray[j] = '\n';
	}

	for(i = 0; i<hauteur; i++){
		for(j = 0; j<largeur; j++){
			global[i][j] = ' ';
		}
		global[i][j] = '\0';
	}

	askedAction[0] = 0;
	askedAction[1] = 0;

	PieceList[0] = LDROIT;
	PieceList[1] = QINVERSE;
	PieceList[2] = T;
	PieceList[3] = QINVERSE;
	PieceList[4] = QUATRE;
	PieceList[5] = LGAUCHE;
	PieceList[6] = BARRE;
	PieceList[7] = QUATRE;
	PieceList[8] = LGAUCHE;
	PieceList[9] = CARRE;
	PieceList[10] = LDROIT;
	PieceList[11] = BARRE;
	PieceList[12] = T;
	PieceList[13] = CARRE;

	sem_action = UserSemCreate("sem_action", 1);
	sem_run = UserSemCreate("sem_action", 0);

	

	//sem_kill = UserSemCreate("sem_kill", 1);
	

	int tidctl = UserThreadCreate(controlThread,(void *) 0);

	printLogo();

	UserThreadJoin(tidctl);

	/*Not reached */
	return 0;
}