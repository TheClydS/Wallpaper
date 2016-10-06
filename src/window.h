/*
 * Name : window.h
 * Auteur : TheClydS
 * Date : 21 septembre 2016
 * Version : 0.1a
 * Description : Programe Wallpaper permet de modifier le fond d'écran
 * 				 Page de définition et prototype
 */
#ifndef __WINDOW_H
#define __WINDOW_H

#include <Imlib2.h>

/* 1) STRUCTURES ET LEURS FONCTIONS */
	/* 1-1) Implémentation des structures */
		typedef struct _Win *Win;
		typedef struct _Img *Img;
		typedef struct _OptAff *OptAff;
	/* 1-2) Allocation de la mémoire pour les  structures */
		//méthode static
	/* 1-3) Initialisation de la structure Win */
		Win initWin(void);
		//Img initImg(void);
		OptAff initOpt(int type, const char * filename, int duree);
	/* 1-4) Libération de l'espace memoire alouée par les structures */
		//méthode static
/* 2) CONTEX */
	/* 2-1) Context GC */
		//méthode static
	/* 2-2) Context IMLIB2 */
		//méthode static
/* 3) Fonction de gestion du programe */
	/* 3-1) Type d'affichage */
		//méthode static
	/* 3-2) Récupération de X */
		//méthode static
	/* 3-3) Lancement */
		void Wallpaper(int argc, char* argv[]);
/*) Annexe */
		int verifArgv(OptAff opt,int argc, char * argv[]);

#endif /*__WINDOW_H*/
