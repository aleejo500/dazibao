#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <string.h>
#include "lect_tlv.c"


int decale(int fd, int src_index, int dst_index, int size){
  char *addr_src, *addr_dst;
  
  if ((addr_src=mmap(NULL, size, PROT_READ, MAP_SHARED, fd, src_index)) == MAP_FAILED){
    perror("mmap on src tlv error");
    return -1;
  }

  if ((addr_dst=mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, dst_index)) == MAP_FAILED){
    perror("mmap on dst position error");
    return -1;
  }

  memcpy(addr_dst, addr_src, size);
  return 0;
}

void compact_tlv(int fd, unsigned char * contenu, int daz_size){
  int i=4; // on passe l'entête
  int j=i, delete_size=0;
  int tlv_size;
  int nexttlv_size;

  while (i<=daz_size){
    if (contenu[i]==0){
      printf("\n Version : %d \n",contenu[i]); 
      printf("Pad1\n");
 
      tlv_size = 1;
      i = i+1;
      delete_size++;

      printf("daz size %d\n", daz_size);
      printf("i %d\n", i);
      printf("delete size %d\n", delete_size);

      decale(fd, i, j, tlv_size);
      printf("i %d\n",i);

      nexttlv_size = calcul_length(contenu, i);
      i += nexttlv_size;
      j += nexttlv_size;
    }

    else if (contenu[i]==1){
      printf("\n Version :");
      printf("TLV PadN :  length MBZ ");
      printf("i %d\n",i);

      tlv_size = calcul_length(contenu,i)+3;
      i += tlv_size;
      delete_size += tlv_size;
      decale(fd, i, j, tlv_size);

      nexttlv_size = calcul_length(contenu, i);
      i += nexttlv_size;
      j += nexttlv_size;
    }

    else {
      tlv_size = calcul_length(contenu,i)+3;

      if (contenu[i]==6)
	tlv_size += 4;

      i+= tlv_size;
      j+= tlv_size;
    }
  }
  printf("%d = %d - %d\n", daz_size-delete_size, daz_size, delete_size);
  if (ftruncate(fd, (daz_size-delete_size)) < 0)
    perror("Truncate daz error");
}

int compact(char * path){
  int fd, size;
  struct stat status;
  unsigned char * contenu;

  if ((fd=open(path, O_RDWR)) < 0)
    perror("open daz to compact error");

  if (fstat(fd,&status) < 0)
    perror("fstat to get daz size error");

  size=status.st_size;
  contenu = malloc(sizeof(char) * size);

  if (read(fd, contenu, size) < 0)
    perror("read daz to get its contents error");

  compact_tlv(fd, contenu, status.st_size);

  free(contenu);
  close(fd);
  return 0;
}

int main(int argc, char ** argv){
  return compact("exemple.dzb");
}
