#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "dazibao_lib.h"

#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)

//struct dazibao * mondaz = (struct dazibao *) malloc(sizeof(dazibao));

int first_menu(){
  // afficher le daz chargé
  // on suppose la structure daz enregistrée et les infos récupérés
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
	int fd,rc,t,r;
	dazibao * result = NULL;
	struct stat finfo;
	
	if ((fd=open(path, O_RDWR, 0666)) < 0){
		perror("open error");
	}

	if(fstat(fd,&finfo)< 0)
	perror("stat error");	

	result = (dazibao *) mmap(path,finfo.st_size,
				  PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANON,-1, 0);
	
	 if (result == MAP_FAILED) perror("mmap error");
	
	// TO DO valider daz(); 
	// TO DO switch case type contenu     
	printf("Quel type de contenu: 2 pour text ");
	scanf("%d", &t);
	//printf("tapez le texte svp... ");
	add_tlv(result,t);

	close(fd);
	return 0;
}



int main(int argc, char ** argv){
  int menu_choice_test;
    //creer_fichier(argv[1]);
  //  charger_daz(argv[1]);	
  menu_choice_test = first_menu();
  exit(EXIT_SUCCESS);
}
