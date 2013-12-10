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
int magia = MAGIC;

//dazibao * un_daz;
int list_tlv_size;
int list_tlv_cpt;

int creer_list_tlv(){
  list_tlv = (tlv*) malloc(SIZE * sizeof(tlv));
  list_tlv_cpt = 0;
  return 0;
}


int add_tlv(dazibao *dazchargee,int type){
  
	tlv *nuevotlv,*courant;
	int l=40,n;
	char *val;
	char buffer[l];
	size_t bytes;
	
	if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
		errno=EAGAIN;
		return -1;
	}

	courant=dazchargee->tlv_fin;
		
	

	//bytes = fread(buffer,12,sizeof(char),stdin);
	//fcntl (STDIN_FILENO, F_SETFL, O_NONBLOCK);
	
	if ((bytes=read(STDIN_FILENO,buffer,l))<=0) {
		perror("read error");
	}

	// to do switch case type
	if ((nuevotlv->value = (char *) malloc (l * sizeof (char))) == NULL) 
		return -1; 
	
	nuevotlv=creer_tlv(type,bytes,buffer);
	 
	printf("newtlv %d %ld %s\n ",nuevotlv->type,nuevotlv->length, nuevotlv->value);
	
	courant->suivant=nuevotlv;
	
	nuevotlv->suivant = NULL; 

	dazchargee->tlv_fin = nuevotlv; 

	dazchargee->size+=nuevotlv->length; 

	//TO Do write tlv to file &  save
	return 0;
}

int calculer_taille(char *cont){//TODO
  return 10;
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
  return nuevotlv->type;
}


int creer_fichier (char * path){
	int fd,rc,w;
	dazibao * result = NULL;
	FILE *fp;
	
	//if ((fd= open("dazibao1.dzb", O_WRONLY|O_CREAT, 0666)) < 0){
	if ((fp= fopen(path, "w+")) ==NULL){
		perror("open error");
		return -1;
	}
	
	rc=creer_dazibao(100);
	
	fseek(fp, 1, SEEK_CUR);
	
	if ((w=fwrite(&magia, sizeof(magia), 2, fp)) < 0)
		perror("write MAGIC 53");
	if ((w=write(fd, "0", 1)) < 0)
		perror("write version 0");
	if ((w=write(fd, "0", 2)) < 0)
		perror("write MBZ 0");
	
	
	close(fd);
	return rc;
}

