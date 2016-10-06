/*
 * Name : fonctions.c
 * Auteur : NICOLAS FILIPPOZZI
 * Date : 30 september 2016
 * Version : 0.1a
 * Description : Page de fonctions
 * 
 */
 #include <stdlib.h>
 #include <stdio.h>
 #include <unistd.h>
 #include <string.h>
 #include <assert.h>
 #include <dirent.h>
 
 #include "fonctions.h"
 
 #define ERREUR -1
 #define ERREUR_CHAR NULL
 #define READMAX 1024

 /* 1) STRUCTURES LIST de char* */
	/* 1-1) Implémentation des structures */
		struct _List{
			int index;
			List next;
			const char *donnee;
		};
	/* 1-2) Allocation de la mémoire pour de la structures LIST*/
		/* 1-3) Fonctions de gestion de la structure*/ 
		//Ajoute un élement à la liste
		List addList(List list,const char * donnee){
			List listTmp=malloc(sizeof(struct _List)+2);//allocList();
			listTmp->index=countListElements(list);
			listTmp->donnee=donnee;
			listTmp->next=list;
			return listTmp;
		}
		//Efface un élément de la liste à la position index
		List delListElement(List list, int index){
			if (list ==NULL)
				return list;
			if(list->index == index){
				List listTmp=list->next;
				free(list);
				listTmp= delListElement(listTmp,index);
				return listTmp;
			}
			else{
				list->next=delListElement(list->next,index);
				return list;
			}
		}
		//Cherche un élément à la position chval
		List chchListElement(List list, int chval){
			List listTmp=list;
			while (listTmp!= NULL){
				if(listTmp->index == chval){
					return listTmp;
				}
				listTmp=listTmp->next;
			}
			return NULL;
		}
		//Compte le nombre d'élément de la list
		int countListElements(List list){		//fonctions récursive
			if(list == NULL)
				return 0;
			return countListElements(list->next)+1;
		}
		//Séléction le index ime élément
		List selectListElement(List list,int index){
			int i=0;
			List listTmp=list;
			for(i=0; i<index && list != NULL;i++){
				listTmp=listTmp->next;
			}
			return listTmp;
		}
		//affiche le competnue de la liste
		void showList(List list){
			List listTmp=list;
			while(listTmp!=NULL){
				printf("Index : %d=>Donnée : %s\n",listTmp->index,listTmp->donnee);
				listTmp=listTmp->next;
			}
		}
		//Vérifie si la liste est vide
		int isVide(List list){
			if(list == NULL)
				return 1;
			else return 0;
		}
		//Retourne la valeur courante de la liste
		int valeurlist(List list){
			return list->index;
		}
		//Retourn les donnee de la liste n°val
		const char* donneeList(List list, int index){
			List listTmp=list;
			while (listTmp!=NULL){
				if(listTmp->index==index)
					return listTmp->donnee;
				listTmp=listTmp->next;
			}
			freeList(listTmp);
			return NULL;
		}
	/* 1-4)Getter */
		const char * getDonnee(List list){return list->donnee;}
		List getNext(List list){return list->next;}
	/* 1-6)Libération de la mémoire */
		List freeList(List list){
			List listTmp=list;
			
			if (list == NULL)
				return NULL;
			else{
				listTmp=list->next;
				free(list);
				return freeList(listTmp);
			}
		}
/* 2) Fonctions diverse */
	/* 2-1)Dossiers*/
		List listRep(const char* path){
			List tmp=NULL;
			DIR* rep;
			char *filename;
				
			rep=opendir(path);
			if(!rep){
				printf("Impossible d'ouvrir le répertoire\n");
				exit(EXIT_FAILURE);
			}
			struct dirent *ent;
			while((ent=readdir(rep))!=NULL){
				printf("Répertoire parcouru,Fichier : %s\n",ent->d_name);
				if((strstr ((const char *)ent->d_name,".png")!=NULL) || (strstr ((const char *)ent->d_name,".jpeg")!=NULL) || (strstr ((const char *)ent->d_name,".jpg")!=NULL)){
					filename=concat((char*)path,concat((char*)"/",ent->d_name));
					tmp=addList(tmp,(const char*)filename);
				}
			}
			closedir(rep);
			tmp=delListElement(tmp,-1);
			showList(tmp);
			return tmp;
		}
	/* 2-2)Fichiers*/
		List listFile(const char *path){
			FILE *fichier = NULL;
			List tmp=NULL;
			int i;
			char donnee[4][READMAX]={ {0},{0},{0} };
			fichier = fopen(path, "r");
			if (fichier != NULL){
				fscanf (fichier, "%s\n%s\n%s", donnee[0],donnee[1],donnee[2]);
				fclose(fichier);
			
				for(i=0; i<3; i++){
					tmp=addList(tmp,(const char*)donnee[i]);
				}
				//tmp=delListElement(tmp,-1);
				showList(tmp);
				return tmp;
			}
			freeList(tmp);
			return NULL;
		}
	/* 2-3)string*/
		//concatenation de deux chaines
		char* concat(char* str1, char* str2){
			assert(str1 != NULL);
			assert(str2 != NULL);
			size_t len1 = strlen(str1);
			size_t len2 = strlen(str2);
			char* res = malloc(len1 + len2 + 1);

			if (res != NULL){
				memcpy(res, str1, len1);
				memcpy(res + len1, str2, len2);
				res[len1 + len2] = 0;
			}
			else{
				printf("Erreur lors de concatenation de %s et %s",str1,str2);
				exit(EXIT_FAILURE);
			}
			return res;
		}
