/*
 * Name : window.h
 * Auteur : NICOLAS FILIPPOZZI
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
		//static Win alloc(void);
		//static Img allocImg(void);
		//static OptAff allocOpt(void);
	/* 1-3) Initialisation de la structure Win */
		Win initWin(void);
		//Img initImg(void);
		OptAff initOpt(int type, const char * filename, int duree);
	/* 1-4) Libération de l'espace memoire alouée par les structures */
		//static void freeWin(Win win);
		//static void freeImg(Img img);
		//static void freeOpt(OptAff opt);
/* 2) CONTEX */
	/* 2-1) Context GC */
		//static GC createGC(Display *dpy,Drawable drawable,unsigned long valueMask,XGCValues gcvalues);
		//static void intGC(Display *dpy,Win win,Pixmap pixmap,GC gc);
		//static void freeGC(Display *dpy,GC gc);
	/* 2-2) Context IMLIB2 */
		//void initImlib(Win win);
		//static void setImage(OptAff opt,Img img,const char* path);
		//static void initOptImlib(Imlib_Image im,Drawable d,int a, int di,int b);
		//static void renderImlib(Imlib_Image im,int x,int y,int w,int h);
		//static void freeImlibImage(Imlib_Image im);
/* 3) Fonction de gestion du programe */
	/* 3-1) Type d'affichage */
		//static void automatic(Img img);
		//static void center(Win win,Img img);
		//static void scaled(Win win,Img img);
		//static void filled(Win win,Img img);
		//static void zoom(Win win,Img img);
		//static void diaporama(Win win,Img img);
		//a venir diaporama;
	/* 3-2) Récupération de X */
		//static Display *initSecondary(Display *dpy,Window *win, int *depth);
		//static void xSecWallpaper(Img img,Win win,Display *dpy,Window root,int depth,unsigned long valueMask, XGCValues gcvalues);
		//static void showWallpaper(Display *dpy,Window root,Pixmap pixmap);
		//static void closeSec(Display *dpy);
	/* 3-3) Lancement */
		void Wallpaper(int argc, char* argv[]);
/*) Annexe */
		int verifArgv(OptAff opt,int argc, char * argv[]);

#endif /*__WINDOW_H*/
