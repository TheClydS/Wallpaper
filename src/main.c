/*
 * Name : wallpaper
 * Auteur : NICOLAS FILIPPOZZI
 * Date : 25 septembre 2016
 * Version : 0.1a
 * Description : Programe Wallpaper permet d'afficher le fond d'ecran
 * 				 -une image de n'importe quel type grace Imlib2
 * 				 -5 types d'affichage au choix : 
 * 						- 0 :Automatique, affiche l'image dans sa taille d'origine et la répete pour remplir tous l'écran
 * 						- 1 :Centre affiche l'image dans sa taille d'origine et la centre
 * 						- 2 :Etiré, affiche l'image avec un modificateur de 1.5
 * 						- 3 :Rempli, affiche l'image à la taille de l'écran
 * 						- 4 :Zoomé, affiche l'image avec un modificateur de 2 et centre l'image, attention l'image peut dépasser l'écran
 * 						- 6 :diporama, affiche l'image en plein écran pendant n seconde
 * 				 -à venir modification de la couleur de fond
 */

#include <stdlib.h>

#include "window.h"

int main( int argc, char *argv[] )
{
	Wallpaper(argc,argv); //lancement du programe
	exit(EXIT_SUCCESS);
}
