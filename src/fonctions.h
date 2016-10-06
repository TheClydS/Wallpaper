/*
 * Name : config.h
 * Auteur : TheClydS
 * Date : 30 september 2016
 * Version : 0.1a
 * Description : Page de fonctions
 * 
 */
 #ifndef __FONCTIONS_H
 #define __FONCTIONS_H
 
/* 1) STRUCTURES LIST de char* */
	/* 1-1) Implémentation des structures */
		typedef struct _List *List;
	/* 1-2) Allocation de la mémoire pour de la structures LIST*/
		//static List allocList(void);
	/* 1-3) Fonctions*/ 
		List addList(List list,const char * donnee);
		List addListEnd(List list,const char* donnee);
		List delListStart(List list);
		List delListElement(List list, int val);
		List delListEnd(List list);
		List selectListElement(List list,int index);
		int valeurlist(List list);
		List chchListElement(List list, int chval);
		int countListElements(List list);
		int countListElementOcurence(List list, int val);
		int isVide(List list);
		const char* donneeList(List list, int val);
		void showList(List list);
	/* 1-4)Getter */
		int getValeur(List list);
		const char * getDonnee(List list);
		List getNext(List list);
	/* 1-4)Libération de la mémoire */
		List freeList(List list);
/* 2) Fonctions diverse */
	/* 2-1)Dossiers*/
		List listFile(const char* path);
	/* 2-2)Fichiers*/
		List listRep(const char *path);
	/* 2-3)string*/
		//concatenation de deux chaines
		char* concat(char* str1, char* str2);

 #endif /*FONCTION_H*/
