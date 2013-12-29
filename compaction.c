#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <string.h>


unsigned char * contenu;


int decale(char * path,int curs, int nb_bytes_a_dec, int size){
	unsigned char *rest;
	int taille_rest,rc,fd,wr,fl;
	taille_rest=size-curs;
	rest = malloc(sizeof(char)*taille_rest);
	free(contenu);
	contenu= malloc(sizeof(char)*size);
	
	
	if ((fd=open(path, O_RDWR, 0666)) < 0)
		perror("open error here");
	
	if ((fl=flock(fd, LOCK_EX)) != 0)
		perror("lock file error in decale");
	
	if ((rc=lseek(fd,curs+nb_bytes_a_dec,SEEK_SET)) < 0)
		perror("seek error");
	if ((rc=read(fd, rest, taille_rest)) < 0)
		perror("read daz to get its contents error");
	
	/*for(i=0;i<=taille_rest;i++){
		printf(" %03X ", rest[i]);
		
	}*/
	printf(" \n\n ");
	
	if ((rc=lseek(fd,curs,SEEK_SET)) < 0)
		perror("seek cur error ");
	
	if ((wr=write(fd, rest, taille_rest)) < 0)
		perror("write daz to get its contents error");
	
	if (ftruncate(fd,size) < 0)
		perror("Truncate daz error");
		close(fd);
	
	if ((fd=open(path, O_RDWR, 0666)) < 0)
		perror("open error here");
		
	if ((rc=read(fd, contenu, size)) < 0)
		perror("read daz to get its contents error");
	
	if ((fl=flock(fd, LOCK_UN)) != 0)
		perror("unlock file error");
	
	/*for(i=0;i<=size;i++){
		printf(" %03X ", contenu[i]);	
	}*/
	
	free(rest);
	close(fd);
	return curs-nb_bytes_a_dec;
}


void compact_tlv(char * path, unsigned char * contenui, int daz_size){
  int i=4; // on passe l'entête
  int j=i, delete_size=0;
  int tlv_size,fl,fd;
  int new_cur;
	contenu= malloc(sizeof(char)*daz_size);
	contenu=contenui;
	
	
	
	while (i<=daz_size){ //calcul bytes a eliminer dans le fichier original
		if (contenui[i]==0){
			tlv_size = 1;
			delete_size++;
		}else if (contenui[i]==1){
			tlv_size = calcul_length(contenui,i)+3;
			delete_size+=tlv_size;
		}else{
			if ((contenui[i] >1) && (contenui[i] < 7)){
				tlv_size = calcul_length(contenui,i)+3;
				if (contenui[i]==6){
					tlv_size += 5;
				}else{ 
					tlv_size+=1;
				}		
			}
		}	
		i+= tlv_size;
	}
	

	//printf("\n elim : %d ii%d\n",delete_size,i);
	i=4;
	tlv_size=0;
	
  while (i<=daz_size && j<=daz_size){
		if (contenu[i]==0){
		  //printf("\n Type : %d  Pad1\n",contenu[i]); 
		  tlv_size = 1;
		  //delete_size++;
		//  printf("delete size %d  i %d  daz size %d \n", delete_size, i,daz_size);
		  new_cur=decale(path, i, tlv_size, daz_size);
		  i=new_cur;
	//	  printf("\ni %d newcur %d \n",i,new_cur);
		}else if (contenu[i]==1){
	//		printf("\n Type : %d  PadN\n",contenu[i]); 
			tlv_size = calcul_length(contenu,i)+4;
			//delete_size+=tlv_size;
		//	printf("delete size %d  i %d  daz size %d \n", delete_size, i,daz_size);
			new_cur=decale(path, i, tlv_size, daz_size);
			i=new_cur;
		//	printf("\ni %d newcur %d \n",i,new_cur);
		}else{
			  if ((contenu[i] >1) && (contenu[i] < 7)){
					tlv_size = calcul_length(contenu,i)+3;
					//printf("\nTLV other : %d ii%d\n",tlv_size,i);
					if (contenu[i]==6){
						tlv_size += 5;
					}else{ 
						tlv_size+=1;
					
					}		
				}
			//printf("\nmie : %d ii%d\n",tlv_size,i);
		}	
	i+= tlv_size;
	j++;
	 
  }
	
	
	
  //printf("%d = %d - %d\n trebi %d", daz_size-delete_size, daz_size, delete_size,taille_rest_bis);
  
	
	
	if ((fd=open(path, O_RDWR, 0666)) < 0)
		perror("open error here");
	
	if ((fl=flock(fd, LOCK_EX)) != 0)
		perror("lock file error in decale");
	
	/*if ((rc=lseek(fd,0,SEEK_END)) < 0)
		perror("seek error");
	*/
	/*if ((rc=read(fd, rest, taille_rest)) < 0)
		perror("read daz to get its contents error");
	
	for(i=0;i<=taille_rest;i++){
		printf(" %03X ", rest[i]);
		
	}*/
	printf(" \n\n ");
	
	if (ftruncate(fd,daz_size-delete_size+1) < 0)
		perror("Truncate daz error");
	
	if ((fl=flock(fd, LOCK_UN)) != 0)
		perror("unlock file error");
	
	close(fd);
	
}

int compact(char * path){
  int fd, rc, fs, fl, size;
  struct stat status;
  unsigned char * contenu;

  if ((fd=open(path, O_RDWR,0666)) < 0)
    perror("open daz to compact error");

  //debut lock
  if ((fl=flock(fd, LOCK_EX)) != 0)
    perror("lock file error");

  if ((fs=fstat(fd,&status)) < 0)
    perror("fstat to get daz size error");

  size=status.st_size;
  contenu = malloc(sizeof(char) * size);

	
	
  if ((rc=read(fd, contenu, size)) < 0)
    perror("read daz to get its contents error");
	
	
	
	if ((fl=flock(fd, LOCK_UN)) != 0)
		perror("unlock file error");
	//fin lock
	close(fd);
	
  compact_tlv(path, contenu, status.st_size);		


  

  free(contenu);
  close(fd);
  return 0;
}

/*int main(int argc, char ** argv){
  return compact("exemple.dzb");
}*/
