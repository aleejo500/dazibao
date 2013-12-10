#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "dazibao_lib.h"

#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)



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
    //creer_fichier(argv[1]);
	charger_daz(argv[1]);	
  exit(EXIT_SUCCESS);
}
