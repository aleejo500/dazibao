#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


int add_pad_n(int fd, int taille, int indice,int type){
  int fl;
  char * zero[taille];
  memset(&zero, 0, taille);

  //printf("\t ind %d  \n", indice);

  if ((fl=flock(fd, LOCK_EX)) < 0)
    perror("lock");
  
  lseek(fd,indice,SEEK_SET);
  if ((write(fd,&type,1)) < 0)
    perror("write");
  /*
  if ((write(fd,&taille,1)) < 0)
    perror("write");
  */
  lseek(fd,3,SEEK_CUR);
  if ((write(fd,&zero, taille)) < 0)
    perror("write");

  if ((fl=flock(fd, LOCK_UN)) < 0)
    perror("unlock");

  return 0;
}

int add_tlv_txt1(dazibao *dazchargee,int type,int fd,int taille,int dated){
	
	tlv *nuevotlv,*courant;
	int l=140,lk;
	unsigned char buffer[l];
	int bytes,newsize;
	unsigned int now=(unsigned int)time(NULL);
	
	if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
		errno=EAGAIN;
		return -1;
	}
	
	courant=dazchargee->tlv_fin;
	
	bytes=read(STDIN_FILENO,buffer,l);
	
	if (bytes<0) {
		perror("read error");
	}
	
	
	if ((nuevotlv->value = (unsigned char *) malloc (l * sizeof(unsigned char))) == NULL) {//w
	//if ((nuevotlv->value = (char *) malloc (bytes*sizeof(char *))) == NULL) {//w
		errno=EAGAIN;
		return -1;
	}
	
	newsize=bytes+taille+5;
	if (dated==6) {
		printf("Timestamp: %d\n",now);
		newsize+=8;
		//type=dated;
		
	}
	nuevotlv=creer_tlv(type,bytes,buffer);
	
	
	//printf("newtlv %d %ld %s\n ",nuevotlv->type,nuevotlv->length, nuevotlv->value);
	

	nuevotlv->suivant = NULL; 
	
	dazchargee->tlv_fin = nuevotlv; 
	dazchargee->size+=nuevotlv->length; 
	

	if ((lk=flock(fd,LOCK_EX)) < 0)
		perror("lock");
	
	if (ftruncate(fd,newsize) < 0)
		perror("Truncate daz error");
	
	
	lseek(fd,taille ,SEEK_SET);
	//lseek(fd,-bytes-3,SEEK_END);
	
	if (dated==6) {
		if((write(fd,&dated,1))<=0) {
			errno=EFAULT;
			perror("write type error");
			close(fd);	
			return -1;
		}
		lseek(fd, 2, SEEK_CUR);
		if((write(fd,&bytes+8,2))<=0) {
			errno=EFAULT;;
			perror("write lenght error");
			close(fd);	
			return -1;
		}
	
	
		lseek(fd, -1, SEEK_CUR);
		if((write(fd,&now,4))<=0) {
			errno=EFAULT;
			perror("write dated error");
			close(fd);	
			return -1;
		}
	}
	

	if((write(fd,&nuevotlv->type,1))<=0) {
		errno=EFAULT;
		perror("write type error");
		close(fd);	
		return -1;
	}
	lseek(fd, 2, SEEK_CUR);
	if((write(fd,&bytes,2))<=0) {
		errno=EFAULT;;
		perror("write lenght error");
		close(fd);	
		return -1;
	}
		
	lseek(fd,-bytes-1,SEEK_END);
	
	if((write(fd,&buffer,nuevotlv->length))<=0) {
		errno=EFAULT;
		perror("write val error");
		close(fd);	
		return -1;
	}
	
	if ((lk=flock(fd,LOCK_UN)) < 0)
		perror("unlock");
	
	if (close(fd)<0)
		perror("close");
	
	return bytes;
}


int add_tlv_picture(dazibao *dazchargee,int type,int fd_daz,int taille,int dated){
	
	tlv *nuevotlv,*courant;
	char buffer[17];
	int fd, rc, lk;
	struct stat st;
	int buf_size,newsize;
	unsigned int now=(unsigned int)time(NULL);
	
	if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
		errno=EAGAIN;
		return -1;
	}
	
	courant=dazchargee->tlv_fin;
	
	scanf("%s", buffer);
	printf("buf %s\n ",buffer);
	if ((fd=open(buffer,O_RDONLY,0666)) < 0){
		perror("open");
		free(buffer);
		return -1;
	}	
	
		if ((lk=flock(fd,LOCK_EX)) < 0)
			perror("lock");
		
		if ((rc=fstat(fd, &st)) < 0)
			perror("stat");
		
		buf_size = st.st_size;
		
		unsigned char buf[buf_size];
		
		if ((rc=read(fd,buf,buf_size)) < 0)
			perror("read");
		
		if ((lk=flock(fd,LOCK_UN)) < 0)
			perror("unlock");
		
		if (close(fd) < 0)
			perror("close");//image
		
			
	
	if ((nuevotlv->value = (unsigned char *) malloc (buf_size * sizeof (unsigned char))) == NULL) 
		return -1; 
	
	newsize=buf_size+taille+5;
	
	if (dated==6) {
		printf("Timestamp: %d\n",now);
		newsize+=8;
		
		
	}
	
	nuevotlv=creer_tlv(type,buf_size,buf);
	
	printf("newtlv %d %ld %s\n ",nuevotlv->type,nuevotlv->length, nuevotlv->value);
	
	
	nuevotlv->suivant = NULL; 
	dazchargee->tlv_fin = nuevotlv; 
	dazchargee->size+=nuevotlv->length; 
	

	
	if ((lk=flock(fd_daz,LOCK_EX)) < 0)
		perror("lock daz");
	
	if (ftruncate(fd_daz,newsize) < 0)
		perror("Truncate daz error");
	
	lseek(fd_daz,taille ,SEEK_SET);
	
	
	if (dated==6) {
		if((write(fd_daz,&dated,1))<=0) {
			errno=EFAULT;
			perror("write type error");
			close(fd_daz);	
			return -1;
		}

		lseek(fd_daz, 2, SEEK_CUR);
		if((write(fd_daz,&nuevotlv->length,2))<=0) {
			errno=EFAULT;
			perror("write lenght error");
			close(fd_daz);	
			return -1;
		}
	
	
		lseek(fd_daz, 0, SEEK_CUR);
		if((write(fd_daz,&now,2))<=0) {
			errno=EFAULT;;
			perror("write dated error");
			close(fd_daz);	
			return -1;
		}
	}
	
	if((write(fd_daz,&nuevotlv->type,1))<=0) {
		errno=EFAULT;
		perror("write type error");
		close(fd_daz);	
		return -1;
	}
	
	lseek(fd_daz, 2, SEEK_CUR);
	if((write(fd_daz,&nuevotlv->length,2))<=0) {
		errno=EFAULT;
		perror("write lenght error");
		close(fd_daz);	
		return -1;
	}
	
	lseek(fd_daz,-nuevotlv->length-1,SEEK_END);
	
	if((write(fd_daz,&buf,buf_size))<=0) {
		errno=EFAULT;
		perror("write val error");
		close(fd_daz);	
		return -1;
	}
	
	
	if ((lk=flock(fd_daz,LOCK_UN)) < 0)
		perror("unlock");
	
	
	if (close(fd_daz)<0)
		perror("close");

	
	return 1;
}


int add_tlv_compound(dazibao *dazchargee,int type,int fd,int taille,int flag){
	
	tlv *nuevotlv,*courant;
	int bytes,newsize,lk;

	
	if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
		errno=EAGAIN;
		return -1;
	}
	
	courant=dazchargee->tlv_fin;
	bytes=0;
	
	
	newsize=taille+4; //entete compound
	nuevotlv=creer_tlv(type,bytes,NULL);
	
	
	printf("newtlv %d %ld %s\n ",nuevotlv->type,nuevotlv->length, nuevotlv->value);
	
	
	nuevotlv->suivant = NULL; 
	dazchargee->tlv_fin = nuevotlv; 
	dazchargee->size+=nuevotlv->length; 
	
	
	if ((lk=flock(fd,LOCK_EX)) < 0)
		perror("lock");
	
	if (ftruncate(fd,newsize) < 0)
		perror("Truncate daz error");
	
	lseek(fd,taille ,SEEK_SET);
	if((write(fd,&nuevotlv->type,1))<=0) {
		errno=EFAULT;
		perror("write type error");
		close(fd);	
		return -1;
	}
	
	lseek(fd, -3, SEEK_END);
	if((write(fd,&bytes,3))<=0) {
		errno=EFAULT;;
		perror("write lenght error");
		close(fd);	
		return -1;
	}
	
		
	if ((lk=flock(fd,LOCK_UN)) < 0)
		perror("unlock");
	
		
	return newsize;
}



