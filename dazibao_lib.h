#define SIZE 10
#define MAGIC 53
#define MAGIC_SIZE sizeof(MAGIC)

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
struct tlv * creer_tlv(int t, int l, unsigned char *v);
char magia = MAGIC;
int list_tlv_size;
int list_tlv_cpt;

int creer_list_tlv(){
  list_tlv = (tlv*) malloc(SIZE * sizeof(tlv));
  list_tlv_cpt = 0;
  return 0;
}

struct tlv *creer_tlv(int t, int l, unsigned char *v){
	if ((t<0) || (t>6))
		return NULL;
	
	struct tlv *newtlv=NULL;
	newtlv = (struct tlv *) malloc(sizeof(struct tlv));
	newtlv->type = t;
	newtlv->length = l;
	newtlv->value = v;
	return newtlv;
}

int creer_fichier (int fd){
	int fl;
	unsigned int bu=0;

	if ((fl=flock(fd, LOCK_EX)) != 0)
	  perror("creer_fichier function, lock");
	
	lseek(fd, 0, SEEK_SET);
	if (write(fd,&magia,sizeof(magia)) <0)
		perror("creer_fichier function, write number MAGIC");
	
	if ( write(fd,&bu,3) <0)
		perror("creer_fichier function, write");

	if ((fl=flock(fd, LOCK_UN)) != 0)
	  perror("creer_fichier function, unlock");

	close(fd);
	return 1;
}

