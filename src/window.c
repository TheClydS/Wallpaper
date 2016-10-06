/*
 * Name : window.c
 * Auteur : TheClydS
 * Date : 22 september 2016
 * Version : 0.1a
 * Description : Programe Wallpaper permet de modifier le fond d'écran
 * 				 Page d'implémentation de window.h
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <Imlib2.h>

#include "config.h"
#include "window.h"
#include "fonctions.h"

/* 1) STRUCTURES ET LEURS FONCTIONS */
	/* 1-1) Implémentation des structures */
		//Implémentation de la structure _Win (infos sur la fenêtre)
		struct _Win{
			Display *dpy;						//Le display
			Screen *scr;						//L'ecran
			Visual *vis;						// pas encore compris...
			Window win;							//la fenetre	
			Colormap cm;						//Mappage des couleur
			int x,y,w,h;						//Décalage de la fenêtre (x,y), taille de la fenêtre(w,h)
			int screen;							//Numéro de l'écran
			int depth;							//Profondeur
		};
		//implémentation de la structure _Img (pixmap, Imlib_Image,x,yw,h)
		struct _Img{
			Pixmap pixmap_1, pixmap_2;			//Objets dessinables
			
			Imlib_Image im;						//L'image
			int x,y,w,h;						//Décalage de la fenêtre d'image en haut à gauche(im_x,im_y), taille de la fenêtre(im_w,im_h)
			int a,di,b;							//antialiasing, dither, blend
		};
		//implémentation de la structure _Opt (type d'affichage, nom de l'image ou chemin du répertoire,durée d'affichage si type = 6
		struct _OptAff{
			int type;							//Type d'affichage
			const char* file;					//Nom du fichier
			int duree;							//Durée d'affichage d'une image si type = 6
		};
	/* 1-2) Allocation de la mémoire pour les  structures */
		//Initialisation de la structure Win
		static Win alloc(void){
			//printf("Fonction alloc.\n");
			Win win;												//Initialisation à null
			
			win=malloc(sizeof(struct _Win)+2);						//Allocation de la mémoire à la structure
			
			win->x=0,win->y=0;win->w=0,win->h=0;					//Valeur par défault
			win->dpy=NULL;											//Valeur par défault
			win->vis=0;												//Initialisation à null
			win->scr=NULL;											//Initialisation à null
			win->win=0;												//Valeur par défault
			win->depth=0;											//Valeur par défault	
			win->cm=0;												//Valeur par défault
			win->screen=0;											//Valeur par défault
				
			//printf("Fonction Alloc fin.\n");
			return win;
		}
		//Initialisation de la structure Img
		static Img allocImg(void){
			//printf("Fonction allocImg\n");
			Img img;
			
			img=malloc(sizeof(struct _Img)+2);
			
			img->pixmap_1=0;										//Initialisation à null
			img->im=NULL;											//Initialisation à null
			img->x=img->y=img->w=img->h=0;							//Valeur par défault
			img->a=img->di=img->b=0;										//Valeur par défault
			
			//printf("Fonction allocImg\n");
			return img;
		}
		//Initialisation de la structure OptAff
		static OptAff allocOpt(void){
			//printf("Fonction allocOpt\n");
			OptAff opt;
			
			opt=malloc(sizeof(struct _OptAff)+2);
			
			opt->type=0;
			opt->file=NULL;
			opt->duree=0;
			
			//printf("Fonction allocOpt fin\n");
			return opt;
		}
	/* 1-3) Initialisation de la structure Win */
		//Initialisation de la structure win
		Win initWin(void){
			//printf("Fonction InitWin.\n");
			Window temp;
			unsigned int border_width;
			Win win=malloc(sizeof(struct _Win)+100);
			win->dpy=XOpenDisplay(NULL);
			if (!win->dpy){
				printf("Impossible d'ouvrir l'affichage X.\n \
				Avez-vous lancer le server X");
				exit(EXIT_FAILURE);
			}
			win->scr=DefaultScreenOfDisplay(win->dpy);
			win->screen=DefaultScreen(win->dpy);
			win->win= RootWindow(win->dpy,DefaultScreen(win->dpy));
			win->vis=DefaultVisual(win->dpy,win->screen);
			win->depth=DefaultDepth(win->dpy,DefaultScreen(win->dpy));
			win->cm=DefaultColormap(win->dpy,DefaultScreen(win->dpy));
			XGetGeometry(win->dpy,win->win, &temp, &win->x,&win->y, (unsigned int *)&win->w, 
                       (unsigned int *)&win->h, &border_width,  (unsigned int *)&win->depth);

			//printf("Fonction initWin fin.\n");
			return win;
		}
		//Inialisation de la structure Img
		//Img initImg(){}
		//Initialisation de la structure OptAff
		OptAff initOpt(int type, const char * filename, int duree){
			//printf("Fonction initOpt\n");
			
			OptAff opt=malloc(sizeof(struct _OptAff)+100);
			
			opt->type=type;
			opt->file=filename;
			opt->duree=duree;
			
			//printf("Fonction initOpt fin\n");
			return opt;
		}
	/* 1-4) Libération de l'espace memoire alouée par les structures */
		//liberation de la memoire pour la structure Win
		static void freeWin(Win win){
			//printf("Libération de la mémoire pour Win\n");
			free(win);
			//printf("Libération de la mémoire pour Win fin\n");
		}
		//liberation de la memoire pour la structure Img
		static void freeImg(Img img){
			//printf("Libération de la mémoire pour Win\n");
			free(img);
			//printf("Libération de la mémoire pour Win fin\n");
		}
		//liberation de la memoire pour la structure OptAff
		static void freeOpt(OptAff opt){
			//printf("Libération de la mémoire pour Win\n");
			free(opt);
			//printf("Libération de la mémoire pour Win fin\n");
		}
/* 2) Context */
	/* 2-1) Context GC */
		//Cré le context gc en fonction des options
		static GC createGC(Display *dpy,Drawable drawable,unsigned long valueMask,XGCValues gcvalues){
			//printf("Fonction createGC\n");
			GC gc;
			//~ XGCValues xgcv;
			//~ xgcv.foreground=BlackPixel(dpy,DefaultScreen(dpy));
			gc=XCreateGC(dpy,drawable, valueMask ,&gcvalues);
			//printf("Fonction createGC fin\n");
			return gc;
		}
		//Intégration du context gc au pixmap
		static void intGC(Display * dpy,Win win,Pixmap pixmap,GC gc){
			//printf("Fonction intGC\n");
			XFillRectangle(dpy,pixmap,gc,win->x,win->y,win->w,win->h);
			//printf("Fonction intGC fin\n");
		}
		//Liberation du context GC
		static void freeGC(Display *dpy,GC gc){
			//printf("Fonction freeGC\n");
			XFreeGC(dpy,gc);
			//printf("Fonction freeGC fin\n");
		}
	/* 2-2) Context IMLIB2 */
		//Initialise le context Imlib2
		void initImlib(Win win){
			//printf("Fonction init_imlib.\n");
			imlib_context_set_display(win->dpy);
			imlib_context_set_visual(win->vis);
			imlib_context_set_colormap(win->cm);
			imlib_context_set_color_modifier(NULL);
			imlib_context_set_progress_function(NULL);
			imlib_context_set_operation(IMLIB_OP_COPY);
			//printf("Fonction init_imblib fin.\n");
		}
		//Chargement de l'image dans le context
		static void setImage(OptAff opt,Img img,const char* path){
			//printf("Fonction setImage.\n");
			const char* filename;
			Imlib_Load_Error err;
			
			if (!opt->file){
				printf("Image ou dossier introuvable\n");
				exit(EXIT_FAILURE);
			}
			filename=(path!=NULL)?path:opt->file;
			img->im=imlib_load_image_with_error_return(filename,&err);
			if ((err) || (!img->im)){
		      switch (err){
					case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
						printf("%s - Le fichier n'existe pas.", filename);
						break;
					case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
						printf("%s - Répertoire spécifié pour l'image.", filename);
						break;
					case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
					   printf("%s - Aucun accés au répertoire.", filename);
					   break;
					case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
					   printf("%s - La librairie Imlib2 ne peut pas chargé ce type de fichier.", filename);
					   break;
					case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
					   printf("%s - Le chemin répertoire du fichier est trop long.", filename);
					   break;
					case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
					   printf("%s - Le chemin répertoire du ficher n'existe pas.", filename);
					   break;
					case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
					   printf("%s - Le chemin du répertoire n'est pas un répertoire.", filename);
					   break;
					case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
					   printf("%s - Path points outside address space.", filename);
					   break;
					case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
					   printf("%s - Trop de redirection symbolique.", filename);
					   break;
					case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
					   printf("Dépassement de mémoire lors du chargement du fichier %s.", filename);
					   break;
					case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
					   printf("While loading %s - Out of file descriptors", filename);
					   break;
					case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
					   printf("%s - Ne peut pas écrie dans ce répertoire.", filename);
					   break;
					case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
					   printf("%s - Ne peut pas écrire dans ce répertoire pas asseé d'espace sur le disque.", filename);
					   break;
					case IMLIB_LOAD_ERROR_UNKNOWN:
					default:
					   printf("While loading %s - Unknown error. Attempting to continue",filename);
					   break;
				}
				exit(EXIT_FAILURE);
			}
			imlib_context_set_image(img->im);
			img->w= imlib_image_get_width();
			img->h= imlib_image_get_height();
			//printf("Fonction setImage fin\n");
		}
		//Option pour une image chargée
		static void initOptImlib(Imlib_Image im,Drawable d,int a, int di,int b){
			//printf("Fonction setImlib.\n");
			imlib_context_set_image(im);
			imlib_context_set_drawable(d);
			imlib_context_set_anti_alias(a);
			imlib_context_set_dither(di);
			imlib_context_set_blend(b);
			imlib_context_set_angle(0);
		}
		//Rendu d'une image chargé réglé
		static void renderImlib(Imlib_Image im,int x,int y,int w,int h){
			//printf("Fonction RenderCenter\n");
			imlib_context_set_image(im);
			imlib_render_image_on_drawable_at_size(x,y,w,h);
			//printf("Fonction RenderCenter fin.\n");
		}
		//~ //Liberation de l'image du context
		//~ static void freeImlibImage(Imlib_Image im){
			//~ //printf("Fonction free\n");
			//~ imlib_context_set_image(im);
			//~ imlib_free_image();
			//~ //printf("Fonction RenderCenter fin.\n");
		//~ }
/* 3) Fonction de gestion du programe */
	/* 3-1) Type d'affichage */
		//Rendu automatique
		static void automatic(Img img){
			//printf("Fonction automatic\n");
				img->x=0;
				img->y=0;
			//printf("Fonction automatic fin\n");
		}
		//Rendu centré
		static void center(Win win,Img img){
			//printf("Fonction center\n");
				img->w=(img->w > win->scr->width)? win->w:img->w;
				img->h=(img->h > win->scr->height)? win->scr->height:img->h;
			
				img->x=((win->w-img->w)/2);
				img->y=((win->h-img->h)/2);
			//printf("Fonction center fin\n");
		}
		//Rendu étiré
		static void scaled(Win win,Img img){
			//printf("Fonction scaled\n");
				img->w=(img->w*1.5 <= win->scr->width)? img->w*1.5:win->w;
				img->h=(img->h*1.5 >= win->scr->height)?img->h*1.5:win->h;
			
				img->x=((win->w-img->w)/2);
				img->y=((win->h-img->h)/2);
			//printf("Fonction scaled fin\n");
		}
		//Rendu Remplir
		static void filled(Win win,Img img){
			//printf("Fonction scaled\n");
				img->w=win->w;
				img->h=win->h;
			
				img->x=0;
				img->y=0;
			//printf("Fonction scaled fin\n");
		}
		//Rendu zoom *2.5
		static void zoom(Win win,Img img){
			//printf("Fonction zoom\n");
				img->w=img->w*2.5;
				img->h=img->h*2.5;
			
				img->x=((win->w-img->w)/2);
				img->y=((win->h-img->h)/2);
			//printf("Fonction zoom fin\n");
		}
		//Rendu diaporama
		static void diaporama(Win win,Img img){
			//printf("Fonction diaporama\n");
			img->w=(img->w > win->scr->width)? win->w:img->w;
			img->h=(img->h > win->scr->height)? win->scr->height:img->h;
			img->x=((win->w-img->w)/2);
			img->y=((win->h-img->h)/2);
			//printf("Fonction diaporama fin\n");
		}
	/* 3-2) Récupération de X */
		//Initialisation de l'affichage pour la mise en arriere plan
		static Display * initSecondary(Display *dpy,Window *win, int *depth){
			////printf("Fonctioncondary\n");
			dpy=XOpenDisplay(NULL);
			if(!dpy){
				printf("Impossible de réouvrir l'afficheur\n");
				exit(EXIT_FAILURE);
			}
			*win=RootWindow(dpy,DefaultScreen(dpy));
			*depth=DefaultDepth(dpy,DefaultScreen(dpy));
			//printf("Fonction initSecondary fin\n");
			return dpy;
		}
		//X secondaire pour la mise en arriére plan
		static void xSecWallpaper(Img img,Win win,Display *dpy,Window root,int depth,unsigned long valueMask, XGCValues gcvalues){
			//printf("Fonction showImg\n");
			GC gc;
			Atom prop_root,prop_esetroot,type;
			prop_root=prop_esetroot=type= None;
			unsigned char *data_root = NULL;
			unsigned char *data_esetroot = NULL;
			//printf("Fin de déclaration des variables\n");
			XSync(win->dpy,False);
			
			//copie du premier pixmap sur un nouveau
			img->pixmap_2=XCreatePixmap(dpy,root,win->w,win->h,depth);
			gc=createGC(dpy,img->pixmap_2,valueMask,gcvalues);
			intGC(dpy,win,img->pixmap_2,gc);
			
			freeGC(dpy,gc);
			XSync(dpy,False);
			XSync(win->dpy,False);
			XFreePixmap(win->dpy,img->pixmap_1);
			
			//Modification des propiétés
			prop_root = XInternAtom(dpy, "_XROOTPMAP_ID", True);
			prop_esetroot = XInternAtom(dpy, "ESETROOT_PMAP_ID", True);
								   
			//libération 
			if (data_root)
				XFree(data_root);
			if (data_esetroot)
				XFree(data_esetroot);
				
			/* This will locate the property, creating it if it doesn't exist */
			prop_root = XInternAtom(dpy, "_XROOTPMAP_ID", False);
			prop_esetroot = XInternAtom(dpy, "ESETROOT_PMAP_ID", False);
			if (prop_root == None || prop_esetroot == None){
				printf("creation of pixmap property failed.");
				exit(EXIT_FAILURE);
			}
			XChangeProperty(dpy, root, prop_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &img->pixmap_2, 1);
			XChangeProperty(dpy, root, prop_esetroot, XA_PIXMAP, 32,PropModeReplace, (unsigned char *) &img->pixmap_2, 1);
			//printf("Fonction showImg fin\n");
		}
		//Affichage et mise en arrière plan
		static void showWallpaper(Display *dpy,Window root,Pixmap pixmap){
			//printf("Fonction showWalpaper\n");
			XSetWindowBackgroundPixmap(dpy, root, pixmap);
			XClearWindow(dpy, root);
			XFlush(dpy);
			XSetCloseDownMode(dpy, RetainPermanent);
			//printf("Fonction showWalpaper fin\n");
		}
		//Fermeture de l'afficheur secondaire
		static void closeSec(Display *dpy){
			//printf("Fonction closeSecDisplay\n");
			XCloseDisplay(dpy);
			//printf("Fonction closeSecDisplay fin\n");
		}
	/* 3-3) Lancement */
		//Réglage du fond d'écran
		void Wallpaper(int argc, char* argv[]){
			//printf("Fonction setWallpaper.\n");
			XGCValues gcvalues;							//Options pour gc
			XGCValues gcval;							//Options pour gc
			GC gc;										//Context Graphique
			
			List list;									//Liste le contenu du répertoire pour le diaporama
			
			OptAff opt=initOpt(-1,NULL,-1);
			verifArgv(opt,argc,argv);
			
			Win win=initWin();
			Img img=allocImg();
			
			Display *dpy=NULL;
			Window root;
			int depth;
			
			initImlib(win);
			dpy=initSecondary(dpy,&root,&depth);
			XSync(win->dpy,False);
			switch(opt->type){
				case AUTOMATIC:							//CAs de selection AUTOMATIQUE
					setImage(opt,img,NULL);
					automatic(img);
					img->pixmap_1=XCreatePixmap(win->dpy,win->win,img->w,img->h,win->depth);
					gcval.background=BlackPixel(win->dpy,DefaultScreen(win->dpy));
					gc=XCreateGC(win->dpy,win->win, GCBackground ,&gcval);
					XFillRectangle(win->dpy,img->pixmap_1,gc,win->x,win->y,win->w,win->h);
			
					initOptImlib(img->im,img->pixmap_1,1,0,0);
					renderImlib(img->im,img->x,img->y,img->w,img->h);
					
					imlib_free_image();
					freeGC(win->dpy,gc);
					break;
				case CENTER:							//CAs de selection Centrer
					setImage(opt,img,NULL);
					center(win,img);
					img->pixmap_1=XCreatePixmap(win->dpy,win->win,win->w,win->h,win->depth);
					gcval.foreground = BlackPixel(win->dpy, win->screen);
					gc=createGC(win->dpy,win->win,GCForeground,gcval);
					intGC(win->dpy,win,img->pixmap_1,gc);
					
					initOptImlib(img->im,img->pixmap_1,1,0,0);
					renderImlib(img->im,img->x,img->y,img->w,img->h);
					
					imlib_free_image();
					freeGC(win->dpy,gc);
					break;
				case SCALED:							//CAs de selection étiré à la taille de l'écran 
					setImage(opt,img,NULL);
					scaled(win,img);
					img->pixmap_1=XCreatePixmap(win->dpy,win->win,win->w,win->h,win->depth);
					gcval.foreground = BlackPixel(win->dpy, win->screen);
					gc=createGC(win->dpy,win->win,GCForeground,gcval);
					intGC(win->dpy,win,img->pixmap_1,gc);
					
					initOptImlib(img->im,img->pixmap_1,1,0,0);
					renderImlib(img->im,img->x,img->y,img->w,img->h);
					
					imlib_free_image();
					freeGC(win->dpy,gc);
					break;
				case FILLED:							//CAs de selection remplir à la taille de l'écran 
					setImage(opt,img,NULL);
					filled(win,img);
					img->pixmap_1=XCreatePixmap(win->dpy,win->win,win->w,win->h,win->depth);
					gcval.foreground = BlackPixel(win->dpy, win->screen);
					gc=createGC(win->dpy,win->win,GCForeground,gcval);
					intGC(win->dpy,win,img->pixmap_1,gc);
					
					initOptImlib(img->im,img->pixmap_1,1,0,0);
					renderImlib(img->im,img->x,img->y,img->w,img->h);
					
					imlib_free_image();
					freeGC(win->dpy,gc);
				case ZOOMED:							//CAs de selection zoom et centré à la taille de l'écran 
					setImage(opt,img,NULL);
					zoom(win,img);
					img->pixmap_1=XCreatePixmap(win->dpy,win->win,win->w,win->h,win->depth);
					gcval.foreground = BlackPixel(win->dpy, win->screen);
					gc=createGC(win->dpy,win->win,GCForeground,gcval);
					intGC(win->dpy,win,img->pixmap_1,gc);
					
					initOptImlib(img->im,img->pixmap_1,1,0,0);
					renderImlib(img->im,img->x,img->y,img->w,img->h);
					
					imlib_free_image();
					freeGC(win->dpy,gc);
					break;
				case DIAPORAMA:
					list=listRep(opt->file);
					while(True){
						while (list != NULL){
							const char *tmp=getDonnee(list);
							printf("fichier : %s\n", tmp);
							setImage(opt,img,tmp);
							diaporama(win,img);
								
							img->pixmap_1=XCreatePixmap(win->dpy,win->win,win->w,win->h,win->depth);
							gcval.foreground = BlackPixel(win->dpy, win->screen);
							gc=createGC(win->dpy,win->win,GCForeground,gcval);
							intGC(win->dpy,win,img->pixmap_1,gc);
								
							initOptImlib(img->im,img->pixmap_1,1,0,0);
							renderImlib(img->im,img->x,img->y,img->w,img->h);
								
							imlib_free_image();
							freeGC(win->dpy,gc);
								
							img->pixmap_2 = XCreatePixmap(dpy, root, win->w, win->h, depth);
							gcvalues.fill_style = FillTiled;
							gcvalues.tile = img->pixmap_1;
							xSecWallpaper(img,win,dpy,root,depth, GCFillStyle | GCTile ,gcvalues);
							showWallpaper(dpy,root,img->pixmap_2);
	
							sleep(opt->duree);
							list=getNext(list);
						}
						list=selectListElement(list,0);
					}
					freeList(list);
					break;
			}
			if(opt->type != 6){
				gcvalues.fill_style = FillTiled;
				gcvalues.tile = img->pixmap_1;
				xSecWallpaper(img,win,dpy,root,depth, GCFillStyle | GCTile ,gcvalues);
				showWallpaper(dpy,root,img->pixmap_2);
			}
			closeSec(dpy);
			
			freeWin(win);
			freeImg(img);
			freeOpt(opt);
			//freeList(list);
			//printf("Fonction setWallpaper fin.\n");
		}
/* 4) Annexe */
		/*Vérification des paramétres du programe*/
		int verifArgv(OptAff opt,int argc, char * argv[]){
			List list=addList(list,NULL);
			char true='F';
			
			if( !argv[1]){
				printf("Aucun paramêtre\n");
				true='T';
			}
			else if(argc>5){
				printf("Utilisation : %s type_affiche fichier_image_ou_répertoire durée_affichage_diaporama\n",argv[0]);
				printf("Taper Wallpaper -h pour afficher l'aide\n");
				exit(EXIT_FAILURE);
			}
			else if(argv[1][0] == '-' && argv[1][1] =='h'){
				printf("Aide du programe Wallpaper\n");
				printf("\033[01;38m Wallpaper\n");
				printf("\033[00;38m [-h] [0]|[1]|[2]|[3]|[4]|[6] [chemin] [duree]\n");
				printf("\033[01;38m DESCRITION\n");
				printf("\033[00;38m Quand elle est invoqué sans l'option, la commande Wallpaper modifie le fond d'écran en fonction du fichier de donnée ($HOME/.config/background/donnee).\n");
				printf("\033[01;38m 0 \033[00;38m Modifie le fond d'écran de manière automatique, répete le fond jusq'à remplir l'ecran.\n");
				printf("\033[01;38m 1 \033[00;38m Modifie le fond d'écran en centrant l'image.\n");
				printf("\033[01;38m 2 \033[00;38m Modifie le fond d'écran,modifie l'image choisie en l'adaptant à la taille de l'écran.\n");
				printf("\033[01;38m 3 \033[00;38m Modifie le fond d'écran modifie l'image choisi en appliquant un zoom  x1.5, redimensionne l'image à la taille de l'écran.\n");
				printf("\033[01;38m 4 \033[00;38m Modifie le fond d'écran, modifie l'image choisie en appliquant un zoom x2.5, centre l'image.\n");
				printf("\033[01;38m 6 \033[00;38m Modifie le fond d'écran, Cré un diaporama des images contenues dans le chemin et l'affiche pendant [duree] seconde.\n");
				exit(EXIT_FAILURE);
			}
			else{
				if(strlen(argv[1])>1){
					printf("Le paramétre du type d'affichage doit contenir un seul chiffre 0,1,2,3,4,6 ou -h\n");
					printf("Taper Wallpaper -h pour afficher l'aide\n");
					exit(EXIT_FAILURE);
				}
				if(!isdigit(argv[1][0])){
					printf("Le premier paramétre doit être un chiffre\n");
					printf("Taper Wallpaper -h pour afficher l'aide\n");
					exit(EXIT_FAILURE);
				}
				switch(atoi(argv[1])){
					case 6:
						if(argc != 4){
							printf("Wallpaper 6 prend deux autres arguments \
Taper Wallpaper -h pour afficher l'aide\n");
							exit(EXIT_FAILURE);
						}
						/*Vérification de argv[3] durée d'affichaque quand argv[1]=6, est un nombre*/
						for(int i=0;i<(int)strlen(argv[3]);i++){
							if(!isdigit(argv[3][i])){
								printf("Le troisieme parametre doit être un nombre exprimé en seconde\n");
							exit(EXIT_FAILURE);
				}
			}
						break;
					default:
						if((atoi(argv[1])!= 0 ) && (atoi(argv[1])!=1) && (atoi(argv[1])!=2) && (atoi(argv[1])!=3) && (atoi(argv[1])!=4)){
							printf("Type d'affichage inconnu :\n");
							printf("Taper Wallpaper -h pour afficher l'aide\n");
							exit(EXIT_FAILURE);
						}
						if(argc !=3){
							printf("Wallpaper %s prend un autre argument de type image\n",argv[1]);
							printf("Taper Wallpaper -h pour afficher l'aide\n");
							exit(EXIT_FAILURE);
						}
						break;			
				}	
			}
			if (true == 'T'){
				list=listFile(concat(getenv("HOME"),(char*)"/.config/background/donnee"));
				opt->type=atoi(donneeList(list,0));
				opt->file=donneeList(list,1);
				opt->duree=atoi(donneeList(list,2));
			}
			else{
				opt->file=argv[2];
				opt->type=atoi(argv[1]);
				opt->duree=(argv[3])?atoi(argv[3]):0;
			}
			if(access(opt->file,F_OK)){
				printf("Le fichier ou répertoire n'existe pas\n");
				exit(EXIT_FAILURE);
			}
			if(access(opt->file,R_OK)){
				printf("Les droits de lecture ne sont pas présent sur : %s\n",argv[2]);
				return 0;
			}
			freeList(list);
			return 1;
}
