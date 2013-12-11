

int calcul_length (unsigned char *tmp,int i ){
  return ((256*256*tmp[i+1])+(256*tmp[i+2])+tmp[i+3]);
}

dazibao *load_daz(unsigned char *tmp, int deb,int taille){
	
  dazibao * result ;
  result = (dazibao *) malloc (sizeof (dazibao));	
  int i=deb;
  int j,k,length=0;
	tlv * debut;
	tlv * fin;
	char *val;

	printf("TLV  :%d  length MBZ ", taille); 
  while(i<taille){
	  tlv * courant = (tlv*) malloc(sizeof(tlv));
    // printf("apres while i= %d taille = %d \n",i,taille);
  
    if(tmp[i]==0) {
		courant->type = 0;
		courant->length = 0;
//      printf("\n Version : %d \n",tmp[i]); 
//      printf("Pad1\n"); 
      i++;
    }
    //tlv
    else if(tmp[i]== 1 ){
		courant->type = 1;
		courant->length = 0;
//      printf("\n Version :");
//      printf("TLV PadN :  length MBZ "); i=i+3;
    }
  
    //Text
    else if(tmp[i]== 2){
		courant->type = 2;
      printf("\n Version : %d \n",tmp[i]);
      printf("Text version \n");
		length=calcul_length(tmp,i);
      i=i+3;
		length=length+i;
		courant->length = length;
      i++;
		val = malloc(sizeof(char)*length);
		k=0;
      for(j=i;j<=length;j++){
		  *(val+(k++))=tmp[j];
		  i++;
	  }
		courant->value = val;
      printf("\n");
    }
  
    //PNG
    else if(tmp[i]== 3){    
      printf("\nVersion : %d \n",tmp[i]);
      length=calcul_length(tmp,i);
      i=i+3;   
      printf("Image PNG :  length : %i  %d\n",length,tmp[i]);
      //length=length+i;
      i=i+length+1;
      printf("\n");    
    }

    //JPEG
    else if(tmp[i]== 4){    
      length=calcul_length(tmp,i);
      printf("\n Version : %d \n",tmp[i]);
      i=i+3;  
      printf("Image JPEG :  length : %i  i= %d\n",length,tmp[i]);
      i=i+length+1;
      printf("\n");    
    }


    //COMPOUND
    else if(tmp[i]== 5){
      printf("\n Version : %d \n",tmp[i]);
      length=calcul_length(tmp,i);
      i=i+3;    
      printf("TLV Compound :  length : %i  version: %d  \n",length,tmp[i+1]);
      length=length+i;
    
   
      load_daz(tmp,i+1,length+1);
      i = i +length+1;
      printf("%d \n",i);      
    }

    //DATE  
    else if(tmp[i]== 6){
      printf("\n Date Version : %d \n",tmp[i]);
      //affiche DATE 
      length=calcul_length(tmp,i);
      i=i+7;
      length=length+i;
      load_daz(tmp,i+1,length);
      i = i +length+1;
    }
  
  }//fin_while(1)
	return result;
}
