#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "dazibao_lib.h"
#include "load_daz.c"
#include "write_in_daz.c"
#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)

//struct dazibao * mondaz = (struct dazibao *) malloc(sizeof(dazibao));

int first_menu(char * path){
  // afficher le daz chargé
  // on suppose la structure daz e	nregistrée et les infos récupérés
  int choice;
  char * buf = malloc(sizeof(char)*2);

  printf("Sélectionner votre choix d'action en entrant le numéro correspondant\n");
  printf("\t1 : Ecrire sur mon dazibao\n");
  printf("\t2 : Supprimer un élément de mon dazibao\n");
  printf("\t3 : Campacter mon dazibao\n");
  
  // read from 0=stdin
//  if (read(0,buf,1) < 0)
  //  perror("Read error");
	scanf("%d", &choice); //change pour scanf a cause du problem de tampon ouvert
  //*(buf+1) = '\0';
 // choice = atoi(t);
  
  switch(choice){
  case 1:
    printf("Your choice is to add a component to your dazibao.\n Aleejo will helpp you. :D\n");
		choice= type_menu(path); 
		first_menu(path);  
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


int type_menu(char * path){
	// afficher le daz chargé
	// on suppose la structure daz e	nregistrée et les infos récupérés
	int choice;
	//char * bufS = malloc(sizeof(char)*2);
	
	
	int fd,rc,t,r,taille,rd;
	unsigned char * buf = NULL;
	char * b;
	dazibao * result = NULL;
	struct stat finfo;
	tlv *nuevotlv = NULL;
	
	if ((fd=open(path, O_RDWR, 0666)) < 0){
		perror("open error here");
	}
	
	if(fstat(fd,&finfo)< 0)
		perror("stat error here");	
	
	taille= finfo.st_size;
	buf = malloc(sizeof(char)*taille);
	if((rd = read(fd,buf,taille)) < 0 ){
		errno=EACCES;
		perror("read error here");
	}

	
	result = load_daz(buf,4,taille);
	
	
	printf("Sélectionner votre type de contenu\n");
	printf("\t1 : pad n\n");
	printf("\t2 : text\n");
	printf("\t3 : image png\n");
	printf("\t4 : image jpg\n");
	printf("\t5 : plusieurs\n");
	printf("\t6 : date \n");
	
	//if (read(0,bufS,1) < 0)
	//	perror("Read error");
	scanf("%d", &choice);
	//*(bufS+1) = '\0';
	//choice = atoi(t);
	
	switch(choice){
		case 1:
			printf("pad n ajoute a la fin du daz\n ");  
			break;
		case 2:
			printf("tapez le texte svp... \n ");
			nuevotlv=add_tlv_txt(result,choice);//return cont ou tlv
			write_tlv(buf,taille,nuevotlv,fd);
			
			break;
		case 3:
			printf("tapez le nom du fichier image... \n ");
			nuevotlv=add_tlv_picture(result,choice);//return cont ou tlv
			write_tlv(buf,taille,nuevotlv,fd);
			break;
		case 4:
			printf("tapez le nom du fichier image... \n ");
			nuevotlv=add_tlv_picture(result,choice);//return cont ou tlv
			
			
			write_tlv(buf,taille,nuevotlv,fd);
			break;
		case 5:
			printf("compound\n");
			break;
		case 6:
			printf("date\n");
			break;
	default:
			
			printf("Error : cannot have another choice than 1 to 6\n");
			return 1;
	}
	result = load_daz(buf,4,taille);
	free(buf);
	close(fd);
	return choice;
}





int main(int argc, char ** argv){
  int menu_choice_test;
    //creer_fichier(argv[1]);
  //  charger_daz(argv[1]);	
  menu_choice_test = first_menu(argv[1]);
  exit(EXIT_SUCCESS);
}
