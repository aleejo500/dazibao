	#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "dazibao_lib.h"
#include "load_daz.c"
#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)

//struct dazibao * mondaz = (struct dazibao *) malloc(sizeof(dazibao));

int first_menu(){
  // afficher le daz chargé
  // on suppose la structure daz e	nregistrée et les infos récupérés
  int choice;
  char * buf = malloc(sizeof(char)*2);

  printf("Sélectionner votre choix d'action en entrant le numéro correspondant\n");
  printf("\t1 : Ecrire sur mon dazibao\n");
  printf("\t2 : Supprimer un élément de mon dazibao\n");
  printf("\t3 : Campacter mon dazibao\n");
  
  // read from 0=stdin
  if (read(0,buf,1) < 0)
    perror("Read error");

  *(buf+1) = '\0';
  choice = atoi(buf);
  
  switch(choice){
  case 1:
    printf("Your choice is to add a component to your dazibao.\n Aleejo will help you. :D\n");
	charger_daz("dazibao.dbz");	  
    break;
  case 2:
    printf("Your choice is to delete a component from your dazibao. :(\n Aleejo will help you.\n");
    break;
  case 3:
    printf("Your choice is to compact your dazibao. You don't need to do a single thing. \nOur linked lists do it by themselves! :D (proud)\n");
    break;
  default:
    printf("Error : cannot have another choice than 1, 2 or 3\n");
    return 1;
  }

  return choice;
}

int charger_daz (char * path){
	int fd,rc,t,r,taille,rd;
	unsigned char * buf = NULL;
	char * b;
	dazibao * result = NULL;
	struct stat finfo;
	
	if ((fd=open(path,  O_RDWR|O_CREAT, 0666)) < 0){
		perror("open error here");
	}

	if(fstat(fd,&finfo)< 0)
	perror("stat error here");	

	taille= finfo.st_size;
	buf = malloc(sizeof(char)*taille);
	if( ( rd = read(fd,buf,taille)) < 0 )
		perror("read error here");
	
	result = load_daz(buf,4,taille);
	
	// TO DO valider daz(); 
	// TO DO switch case type contenu     
	printf("Quel type de contenu: 2 pour text \n");
	scanf("%d", &t);
	
	//read(0,b, 1);

	printf("tapez le texte svp... \n ");
	//add_tlv(result,t);

	close(fd);
	return 0;
}



int main(int argc, char ** argv){
  int menu_choice_test;
    //creer_fichier(argv[1]);
    charger_daz(argv[1]);	
  //menu_choice_test = first_menu();
  exit(EXIT_SUCCESS);
}
