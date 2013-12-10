#define MBZ 0

int lect_entete(unsigned char*tmp){
  // test en tête
  if( (tmp[0]==53) && (tmp[1]==0) && MBZ==(256*tmp[2]+tmp[3]))
    printf("MAGIC 53  version:0  MBZ : %i \n",(256*tmp[2]+tmp[3]));
  else 
    return -1;
}
