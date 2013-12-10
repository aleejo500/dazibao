int calcul_length (unsigned char *tmp,int i ){
  return ((256*256*tmp[i+1])+(256*tmp[i+2])+tmp[i+3]);
}
void lect_tlv(unsigned char *tmp, int deb,int taille){
  int i=deb;
  int j,length=0;



  while(i<taille){
    printf("i= %d taille = %d \n",i,taille);
  printf("tlv \n");
  //tlv
  if(tmp[i]== '1' ){
    printf(" TLV PadN :  length MBZ " ); i=i+3;
  }
  
  //Text
  if(tmp[i]== 2){
    length=calcul_length(tmp,i);
    i=i+3;
    length=length+i;
    printf("i= %d\n",i);

    for(j=i;j<length;j++){printf("%c",tmp[j]);i++;}
    printf("\n");
  }
  //PNG
  if(tmp[i]== '3'){    
    length=calcul_length(tmp,i);
    i=i+3;    
    printf("Image PNG :  length : %i  %d\n",length,i);
    length=length+i;
    i=length;
    printf("\n");    
  }

  //COMPOUND
  if(tmp[i]== '5'){
    length=calcul_length(tmp,i);
    i=i+3;    
    printf("TLV Compound :  length : %i  %d\n",length,i);
    length=length+i;
    
    while(i<length) {
      lect_tlv(tmp,i,length);
      /*lecture_dzb(d,i);
	i++;*/
    }
    printf("%d \n",i);      
  }
  //DATE  
  if(tmp[i]== '6')
  printf("Date \n");
  
  
  }//fin_while(1)
  
  
  

}
