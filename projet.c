#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "dazibao_lib.h"

#define MAGIC "53"
#define MAGIC_SIZE sizeof(MAGIC)

int creer_fichier (char * path){
  int fd,rc,w;
  dazibao * result = NULL;

 if ((fd= open("dazibao.dzb", O_WRONLY|O_CREAT, 0666)) < 0){
    perror("open error");
  }

  creer_dazibao(100000);

  rc = lseek(fd, 8, SEEK_SET);
  
  if ((w=write(fd, MAGIC, MAGIC_SIZE)) < 0)
    perror("write MAGIC 53");
  if ((w=write(fd, "0", 1)) < 0)
    perror("write version 0");
  if ((w=write(fd, "0", 2)) < 0)
    perror("write MBZ 0");
  
  
  close(fd);
  return 0;
}



int main(int argc, char ** argv){
   creer_fichier(argv[1]);
 
  exit(EXIT_SUCCESS);
}
