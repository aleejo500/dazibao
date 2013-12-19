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
  int type;
  long length;
  char * value;
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



tlv *add_tlv_txt(dazibao *dazchargee,int type){
  
	tlv *nuevotlv,*courant;
	int l=40,n;
	char *val;
	char buffer[l];
	int bytes;
	
	if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
		errno=EAGAIN;
		return NULL;
	}

	courant=dazchargee->tlv_fin;
		
	bytes=read(STDIN_FILENO,buffer,l);
	
	if (bytes<0) {
		perror("read error");
	}

	
	if ((nuevotlv->value = (char *) malloc (l * sizeof (char))) == NULL) 
		return NULL; 
	
	nuevotlv=creer_tlv(type,bytes,buffer);
	 
	
	printf("newtlv %d %ld %s\n ",nuevotlv->type,nuevotlv->length, nuevotlv->value);
	
	//courant->suivant=nuevotlv;

	nuevotlv->suivant = NULL; 
	
	dazchargee->tlv_fin = nuevotlv; 
	dazchargee->size+=nuevotlv->length; 
//	printf("suivant3\n ");
	//TO Do write tlv to file &  save
	return (tlv*) nuevotlv;
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
  int fd,rc,w;
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


int creer_fichier (char * path){
	int fd,rc,w;
	dazibao * result = NULL;
	FILE *fp;
	unsigned int bu=0;
	if ((fd= open(path, O_WRONLY|O_CREAT, 0666)) < 0)
		perror("open error");
	
	if ( write(fd,&magia,sizeof(magia)) <0)
		perror("write error");
	
	lseek(fd, 4, SEEK_SET);
	if ( write(fd,&bu,sizeof(bu)) <0)
		perror("write error");
	
	
/*
	if ((fp= fopen(path, "w+")) ==NULL){
		perror("open error");
		return -1;
	}
	
    rc=creer_dazibao(100);
	fseek(fp, 0, SEEK_SET);
	if ((w=fwrite(&magia, sizeof(magia), 1, fp)) < 0)
		perror("write MAGIC 53");
	if ((w=write(fd, "0", 1)) < 0)
		perror("write version 0");
	if ((w=write(fd, "0", 2)) < 0)
		perror("write MBZ 0");
	*/
	
	close(fd);
	return rc;
}

tlv *add_tlv_picture(dazibao *dazchargee,int type){
	
	tlv *nuevotlv,*courant;
	int l=40,fd,taille,rd;
	unsigned char *val;
	char buffer[l];
	int bytes;
	struct stat finfo;
	
	if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
		errno=EAGAIN;
		return NULL;
	}
	
	courant=dazchargee->tlv_fin;
	bytes=read(STDIN_FILENO,buffer,l);
	
	if (bytes<0) {
		perror("read error");
	}
	
	
	if ((fd=open(buffer, O_RDWR, 0666)) < 0){
		perror("open error here");
	}
	
	//to do extraire donnees
	if(fstat(fd,&finfo)< 0)
		perror("stat error ims");	
	
	taille= finfo.st_size;
	val = malloc(sizeof(char)*taille);
	if((rd = read(fd,val,taille)) < 0 ){
		errno=EACCES;
		perror("read error here");
	}
	
	
	
	if ((nuevotlv->value = (char *) malloc (l * sizeof (char))) == NULL) 
		return NULL; 
	
	nuevotlv=creer_tlv(type,bytes,buffer);
	
	
	printf("newtlv %d %ld %s\n ",nuevotlv->type,nuevotlv->length, nuevotlv->value);
	
	//courant->suivant=nuevotlv;
	
	nuevotlv->suivant = NULL; 
	
	dazchargee->tlv_fin = nuevotlv; 
	dazchargee->size+=nuevotlv->length; 
	//	printf("suivant3\n ");
	//TO Do write tlv to file &  save
	return (tlv*) nuevotlv;
}


