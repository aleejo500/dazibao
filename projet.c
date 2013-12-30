#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include "dazibao_lib.h"
#include "load_daz.h"
#include "ajout.h"
#include "compaction.h"

#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)

int taille_comp=0;

/**
 * Menu pour selectionner le type de donnee a ajouter
 */
int type_menu(char * path,int opc){
  int choice;
  int fd,r=0,taille,rd,numcomp,i=0;
  unsigned char * buf = NULL;
  dazibao * result = NULL;
  struct stat finfo;
	
  if ((fd=open(path, O_RDWR, 0666)) < 0){
    perror("type_menu function, open");
  }
	
  if(fstat(fd,&finfo)< 0)
    perror("typ_menu function, stat");	
	
  taille= finfo.st_size;
  buf = malloc(sizeof(char)*taille);
  if((rd = read(fd,buf,taille)) < 0 ){
    errno=EACCES;
    perror("type_menu function, read");
  }

  result = load_daz(buf,4,taille);
  printf("Select the type of the content you want to add\n");
  //printf("\t1 : PadN\n");
  printf("\t2 : Text\n");
  printf("\t3 : Picture .png\n");
  printf("\t4 : Picture .jpg\n");
  printf("\t5 : Multiple TLVs\n");
  printf("\t6 : Dated Content\n");
	
  scanf("%d", &choice);
  if (choice>4 && opc>0)
    opc=choice;
 
  switch(choice){
  
  case 2:
    printf("Please enter your text... \n ");
    r=add_tlv_txt1(result,2,fd,taille,opc);
    taille_comp+=r;	
    break;
  case 3:
    printf("Please entre the file's name... \n ");
    r=add_tlv_picture(result,choice,fd,taille,opc);
    taille_comp+=r;
    break;
  case 4:
    printf("Please enter the file's name... \n ");
    r=add_tlv_picture(result,choice,fd,taille,opc);
    taille_comp+=r;
    break;
  case 5:
    if (opc<=4) {
      taille_comp=0;
      printf("Compound %d\n", taille_comp);
      printf("Please enter the number of contents you want to add... \n ");
      scanf("%d", &numcomp);
      r=add_tlv_compound(result,choice,fd,taille,opc);
      while (i<numcomp) {
	printf("Choose between options 2 3 and 4\n");
	type_menu(path,choice);
	i++;
      }
      printf("Cd %d\n", taille_comp);
      lseek(fd, taille+1, SEEK_SET);
      if((write(fd,&taille_comp,3))<=0) {
	perror("type_menu function, write compound's size");
	close(fd);	
	return -1;
      }
      if (close(fd)<0)
	perror("type_menu function, close");			  
    } else { printf("Choice error :(\n"); }
    break;
  case 6:
    if (opc<=4) {
      printf("Dated content\n");
      printf("Choose between options 2 3 and 4\n");
      type_menu(path,choice);
    } else { printf("Choice error :(\n"); }
    break;
  default:
    printf("Error : cannot have a choice other than 1 to 6\n");
    close(fd);
    return 1;
  }
  free(buf);
  close(fd);
  return choice;
}

int verif_entete(char *tmp){
  int fd,rd,taille;
  unsigned char * buf = NULL;
  struct stat finfo;
	
  if ((fd=open(tmp, O_RDONLY, 0666)) < 0)
    perror("verif_entete function, open");

  if(fstat(fd,&finfo)< 0)
    perror("verif_entete function, fstat");	
	
  taille= finfo.st_size;
  buf = malloc(sizeof(char)*taille);
  if((rd = read(fd,buf,taille)) < 0 ){
    errno=EACCES;
    perror("verif_entete function, read");
  }	
  if(buf[0]!=53) {
    printf("Ouch entête inconnue, à bientôt!\n");  
    return -1;
  }
  else {
    if(buf[1]!=0) {
      printf("Ouch entête inconnue, à bientôt!\n");  
      return -1;
    }
  }
  printf("Nothing wrong with your dazibao.. \n"); 
  return 1;	
}

int voir_daz(char * path){
  int fd,rd,taille;
  unsigned char * buf = NULL;
  struct stat finfo;

  if ((fd=open(path, O_RDONLY)) < 0)
    perror("voir_daz function, open");
	
  if (flock(fd, LOCK_EX) < 0)
    perror("voir_daz function, lock");

  if (fstat(fd,&finfo)< 0)
    perror("voir_daz function, fstat");	
	
  taille = finfo.st_size;
  buf = malloc(sizeof(char)*taille);
  if((rd = read(fd,buf,taille)) < 0 ){
    errno=EACCES;
    perror("voir_daz function, read");
  }

  if (flock(fd, LOCK_UN) < 0)
    perror("voir_daz function, unlock");

  rd = load_daz1(buf,4,taille);
  close(fd);
  return rd;
}

int first_menu(char * path){
  int choice,n=0,to_del;
 
  printf("\t Select your action by using the numbers\n");
  printf("\t0 : Have a look at my dazibao\n");	
  printf("\t1 : Write something on dazibao\n");
  printf("\t2 : Delete something from dazibao\n");
  printf("\t3 : Campact my dazibao\n");
  printf("\t4 : Exit\n");	
  scanf("%d", &choice); 
  
  switch(choice){
  case 0:
    printf("Check your dazibao out.\n ");
    n=voir_daz(path); 
    printf("Total TLVs-> %d.\n ",n);  
    break;	  
  case 1:
    printf("Add a component to your dazibao.\n ");
    type_menu(path,0); 
    break;
  case 2:
    printf("Delete a component from your dazibao. Which one?\n");
    scanf("%d", &to_del);
    del(path,to_del);
    break;
  case 3:
    printf("Compact your dazibao.\n");
    compact(path);	  	  
    break;
  case 4:
    exit(EXIT_SUCCESS);
    break;	  
  default:
    printf("Error : can only have 0, 1, 2 or 3\n");
    return -1;
  }
  first_menu(path);
  return choice;
}

int existe(char * path){
  if ((access(path, F_OK)) == 0){
    return 1;
  }
  else {
    return 0; 
  }
}

int isDZB(char * path){
  char *extension;
  extension = strrchr(path, '.');
  if (strncmp(extension+1, "dzb", 3) == 0){
    return 1;
  }
  else {
    return 0;
  }
}

char *propose_creation(){ //on charge ou cree un dazibao s'il n'existe pas
  int  rc,fd;
  char *name;
  int BUFFER_SIZE=17;
  char *buf = malloc(sizeof(char)*BUFFER_SIZE);
	
  printf("You forgot your dazibao file's name.\n");	
  printf("Entrez the name\n");
  if ((rc=read(STDIN_FILENO, buf, BUFFER_SIZE)) < 0)
    perror("propose_creation function, read");
	
  name = malloc(sizeof(char) * rc-1);
  strncpy(name, buf, rc-1);
	
  if ((fd= open(name, O_RDONLY)) < 0){
    errno=ENOENT;	
    perror("propose_creation function, open");
    printf("\t unknown dazibao\n");
    if ((fd= open(name, O_CREAT|O_RDWR,0666)) < 0){
      errno=EACCES;	
      perror("propose_creation function, open");	
      return NULL;
    }else {	
      printf("\t Created new dazibao\n");
      creer_fichier(fd);
      close(fd);
    }	
  }else {
    printf("\t Known and opened dazibao\n");
    close(fd);
    return name;
  }
  return name;
}

int main(int argc, char ** argv){
  int menu_choice_test;
  char *name;
	
  if (argc < 2){
    name = propose_creation();
  } else {
    if (existe(argv[1]) <= 0) {
      name = propose_creation();
    }
    name = argv[1];
  }
  if (verif_entete(name) >= 0)
    menu_choice_test = first_menu(name);
	
  exit(EXIT_SUCCESS);
}
