#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <time.h>
#include "dazibao_lib.h"
#include "load_daz.c"
#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)

//struct dazibao * mondaz = (struct dazibao *) malloc(sizeof(dazibao));

int first_menu(char * path){
  // afficher le daz chargé
  // on suppose la structure daz e	nregistrée et les infos récupérés
  int choice;
 
  printf("\t Sélectionner votre choix d'action en entrant le numéro correspondant\n");
  printf("\t0 : Voir mon dazibao\n");	
  printf("\t1 : Ecrire sur mon dazibao\n");
  printf("\t2 : Supprimer un élément de mon dazibao\n");
  printf("\t3 : Campacter mon dazibao\n");
  
  
	scanf("%d", &choice); //change pour scanf a cause du problem de tampon ouvert
  
  switch(choice){
  case 0:
	  printf("Your choice is seen your dazibao.\n ");
	  choice= voir_daz(path); 
	  first_menu(path);  
	  break;
		  
  case 1:
    printf("Your choice is to add a component to your dazibao.\n ");
		choice= type_menu(path,0); 
		first_menu(path);  
    break;
  case 2:
    printf("Your choice is to delete a component from your dazibao. \n");
    break;
  case 3:
    printf("Your choice is to compact your dazibao. You don't need to do a single thing. \n\n");
    break;
  default:
    printf("Error : cannot have another choice than 0, 1, 2 or 3\n");
    return 1;
  }

  return choice;
}


int type_menu(char * path,int opc){
	// afficher le daz chargé
	// on suppose la structure daz e	nregistrée et les infos récupérés
	int choice;
	int fd,rc,t,r,taille,rd,numcomp,i=0;
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
	
	
	printf("Sélectionner le type de contenu à ajouter\n");
	printf("\t1 : Pad N.\n");
	printf("\t2 : Text\n");
	printf("\t3 : Image png\n");
	printf("\t4 : Image jpg\n");
	printf("\t5 : Plusieurs\n");
	printf("\t6 : Contenu Date \n");
	
	scanf("%d", &choice);
	
	if (choice>4 && opc>0) {
			opc=choice;
	}

	
	
	switch(choice){
		case 1:
			printf("Pad N ajouter a la fin du dazibao\n ");  
			break;
		case 2:
			printf("Tapez le texte svp... \n ");
			r=add_tlv_txt1(result,2,fd,taille,opc);//bu
			break;
		case 3:
			printf("Tapez le nom du fichier image... \n ");
			r=add_tlv_picture(result,choice,fd,taille,opc);
			break;
		case 4:
			printf("Tapez le nom du fichier image... \n ");
			r=add_tlv_picture(result,choice,fd,taille,opc);
			break;
		case 5:
			if (opc<=4) {
				printf("Compound\n");
				printf("Tapez le nombre d'elements à ajouter... \n ");
				scanf("%d", &numcomp);
				while (i<numcomp) {
					printf("Choisissez entre l'option 2 3 et 4\n");
					type_menu(path,choice);
					i++;
				}
			}else printf("Mauvaise choix :(\n");	
			break;
		case 6:
			if (opc<=4) {
				printf("Contenu daté\n");
				printf("Choisissez entre l'option 2 3 et 4\n");
				type_menu(path,choice);
			}else	printf("Mauvaise choix :(\n");	
			break;
	default:
			
			printf("Error : cannot have another choice than 1 to 6\n");
			return 1;
	}
	//result = load_daz(buf,4,taille);
	free(buf);
	close(fd);
	return choice;
}

int voir_daz(char * path){
		
	int fd,rd,taille;
	unsigned char * buf = NULL;
	dazibao * result = NULL;
	struct stat finfo;

	
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
		
	result = load_daz1(buf,4,taille);
	return 1;
}

int main(int argc, char ** argv){
  int menu_choice_test;
    //creer_fichier(argv[1]);
  //  charger_daz(argv[1]);	
  menu_choice_test = first_menu(argv[1]);
  exit(EXIT_SUCCESS);
}
