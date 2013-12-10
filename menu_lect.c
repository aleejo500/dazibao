#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include "lect_tlv.c"
#include "helpers.c"
#include "lect_entete.c"


int main(int argc , char ** argv){
  unsigned char *buf = NULL;
  int fd,taille,rd;
  struct stat s;
  if(argc<2)
    err_champ("\n usage :- ./menu \"fichier.dzb \" \n");  

  if((fd=open(argv[1] ,O_RDONLY))<0)
    err_sys("open error");
  
  if(fstat(fd,&s) < 0)
    err_sys("fstat");
  taille= s.st_size;
  buf = malloc(sizeof(char)*taille);
  if( ( rd = read(fd,buf,taille) ) < 0 )
    err_sys("read error");
  printf("Taille est %d && nb_lu est %d \n ",taille,rd);
  //Test: entete_dazibao
  if(lect_entete(buf)<0)
    err_sys("entete dazibao non valide");
  //TLV
  lect_tlv(buf,4,taille);

  
  free(buf);  
  close(fd);
    
   
  
  
  return 0;
  
}
