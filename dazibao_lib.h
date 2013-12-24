#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)
#define SIZE 10

typedef struct tlv{
  unsigned int type;
  unsigned long length;
  unsigned char *value;
  struct tlv * suivant;

}tlv;

typedef struct dazibao{
	char * path;
	size_t size;
	int magic;
	int version;
	int mbz;
	tlv * tlv_deb;
	tlv * tlv_fin;		
}dazibao;

struct tlv *list_tlv;
struct tlv * creer_tlv(int t, int l, char *v);
char magia = MAGIC;

//dazibao * un_daz;
int list_tlv_size;
int list_tlv_cpt;

int creer_list_tlv(){
  list_tlv = (tlv*) malloc(SIZE * sizeof(tlv));
  list_tlv_cpt = 0;
  return 0;
}



int add_tlv_txt1(dazibao *dazchargee,int type,int fd,int taille,int dated){
	
	tlv *nuevotlv,*courant;
	int l=140,lk;
	char buffer[l];
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
	
	
	if ((nuevotlv->value = (char *) malloc (l * sizeof(char))) == NULL) {//w
	//if ((nuevotlv->value = (char *) malloc (bytes*sizeof(char *))) == NULL) {//w
		errno=EAGAIN;
		return -1;
	}
	
	newsize=bytes+taille+5;
	if (dated==6) {
		printf("Timestamp: %d\n",now);
		newsize+=4;
		type=dated;
		
	}
	nuevotlv=creer_tlv(type,bytes,buffer);
	
	
	printf("newtlv %d %ld %s\n ",nuevotlv->type,nuevotlv->length, nuevotlv->value);
	

	nuevotlv->suivant = NULL; 
	
	dazchargee->tlv_fin = nuevotlv; 
	dazchargee->size+=nuevotlv->length; 
	

	
	
	if ((lk=flock(fd,LOCK_EX)) < 0)
		perror("lock");
	
	if (ftruncate(fd,newsize) < 0)
		perror("Truncate daz error");
	
	
	
	
	lseek(fd,taille ,SEEK_SET);
	//lseek(fd,-bytes-3,SEEK_END);
	
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
	
	if (dated==6) {
		lseek(fd, 0, SEEK_CUR);
		if((write(fd,&now,4))<=0) {
			errno=EFAULT;
			perror("write dated error");
			close(fd);	
			return -1;
		}
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
	
	return 1;
}



struct tlv *creer_tlv(int t, int l, char *v){
	if ((t<0) || (t>6))
		return NULL;
	
	struct tlv *newtlv=NULL;
	newtlv = (struct tlv *) malloc(sizeof(struct tlv));
	newtlv->type = t;
	newtlv->length = l;
	newtlv->value = v;
	
	return newtlv;
}



int creer_dazibao(long taille){
  
  tlv *nuevotlv;
  dazibao * un_daz;	
  un_daz = (dazibao *) mmap(NULL,taille*sizeof(dazibao),
			    PROT_READ | PROT_WRITE,
			    MAP_PRIVATE | MAP_ANON,-1, 0);

  if (un_daz == MAP_FAILED) perror("mmap error");

  un_daz->magic = MAGIC;
  un_daz->version = 0;
  un_daz->mbz = 0;

 
	nuevotlv=creer_tlv(2,5,"Hola");
	un_daz->tlv_deb = nuevotlv;	
	un_daz->tlv_fin = nuevotlv;
	nuevotlv->suivant=NULL;	
  //TO Do write tlv to file
	
  printf("list undaz: tdaz=%p, len=%ld\n  nuevotlv,%ld\n", un_daz,taille*sizeof(dazibao), nuevotlv->length);
  return (nuevotlv->type);
}


int creer_fichier (int fd){

	int fl;
	unsigned int bu=0;
	
	  // debut lock

	if ((fl=flock(fd, LOCK_EX)) != 0)
	  perror("lock file error");
	
	if (write(fd,&magia,sizeof(magia)) <0)
		perror("write error");
	
	lseek(fd, 2, SEEK_SET);
	if ( write(fd,&bu,sizeof(bu)) <0)
		perror("write error");


 //fin lock

	if ((fl=flock(fd, LOCK_UN)) != 0)
	  perror("unlock file error");

	close(fd);
	return 1;
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
		
		char buf[buf_size];
		
		if ((rc=read(fd,buf,buf_size)) < 0)
			perror("read");
		
		if ((lk=flock(fd,LOCK_UN)) < 0)
			perror("unlock");
		
		if (close(fd) < 0)
			perror("close");//image
		
			
	
	if ((nuevotlv->value = (char *) malloc (buf_size * sizeof (char))) == NULL) 
		return -1; 
	
	newsize=buf_size+taille+5;
	
	if (dated==6) {
		printf("Timestamp: %d\n",now);
		newsize+=4;
		type=dated;
		
		
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
	
	if (dated==6) {
		lseek(fd_daz, 0, SEEK_CUR);
		if((write(fd_daz,&now,2))<=0) {
			errno=EFAULT;;
			perror("write dated error");
			close(fd_daz);	
			return -1;
		}
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


