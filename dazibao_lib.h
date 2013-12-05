#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

#define MAGIC "53"
#define MAGIC_SIZE sizeof(MAGIC)
#define SIZE 10

typedef struct tlv{
  int type;
  int length;
  char * value;
}tlv;

typedef struct {
  char * path;
  int size;
  char *magic;
  int version;
  int mbz;
  struct tlv * tlvs;
}dazibao;

struct tlv *list_tlv;
dazibao * un_daz;
int list_tlv_size;
int list_tlv_cpt;

int creer_list_tlv(){
  list_tlv = (tlv*) malloc(SIZE * sizeof(tlv));
  // memset(&list_tlv, NULL, SIZE*sizeof(tlv));
  list_tlv_cpt = 0;
  return 0;
}

int add_tlv(tlv * t){
  if (list_tlv_cpt > list_tlv_size){
    //    elargir(list_tlv);  TODO
  }
  list_tlv[list_tlv_cpt] = *t;
  return 0;
}

int creer_tlv(int t, int l, char * v){
  if ((t<0) || (t>6))
    return 1;

  struct tlv * newtlv;
  newtlv = (struct tlv *) malloc(sizeof(tlv));
  newtlv->type = t;
  newtlv->length = l;
  newtlv->value = v;
  add_tlv(newtlv);

  return 0;
}


int creer_dazibao(long taille){
  int fd,rc,w;
 
  un_daz = (dazibao *) mmap(NULL,taille*sizeof(dazibao),
			    PROT_READ | PROT_WRITE,
			    MAP_PRIVATE | MAP_ANON,-1, 0);

  if (un_daz == MAP_FAILED) perror("mmap error");

  un_daz->magic = MAGIC;
  un_daz->version = 0;
  un_daz->mbz = 0;

  creer_list_tlv();
  un_daz->tlvs = list_tlv;
  creer_tlv(2,3,"ola");

  printf("list undaz: tdaz=%p, len=%ld\n", un_daz,taille);
  return 0;
}
