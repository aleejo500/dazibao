

int calcul_length (unsigned char *tmp,int i ){
  return ((256*256*tmp[i+1])+(256*tmp[i+2])+tmp[i+3]);
}

time_t calcul_date(unsigned char *tmp,int i)
{
	return( (256*256*256*tmp[i+1])+(256*256*tmp[i+2])+(256*tmp[i+3])+tmp[i+4] );
}

time_t calcul_datebige(unsigned char *tmp,int i)
{
	return( tmp[i+1]+(256*tmp[i+2])+(256*256*tmp[i+3])+(256*256*256*tmp[i+4]) );
}

// fonction qui convertit et affiche la date
void affiche_date(time_t date){
	struct tm *tlv_date;
	tlv_date = localtime(&date);
	printf("\t%d-%d-%d \n",tlv_date->tm_mday,tlv_date->tm_mon +1,1900+ tlv_date->tm_year);
}


dazibao *load_daz(unsigned char *tmp, int deb,int taille){
	
  dazibao * result ;
  result = (dazibao *) malloc (sizeof (dazibao));	
  int i=deb;
  int j,k,length=0;
//	tlv * debut;
//	tlv * fin;
	char *val;

	printf("\nTaille: %d  MBZ\n ", taille); 
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
		//printf("\n Type : %d \n",tmp[i]);
		//printf(" Text version \n");
		length=calcul_length(tmp,i);
		 //printf("tataille :  length : %i  %d\n",length,tmp[i]);
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
		//printf("\n --> %s",courant->value );
    }
  
    //PNG
    else if(tmp[i]== 3){    
      //printf("\n Type : %d \n",tmp[i]);
      length=calcul_length(tmp,i);
      i=i+3;   
      //printf("Image PNG :  length : %i  %d\n",length,tmp[i]);
      //length=length+i;
      i=i+length+1;
      //printf("\n");    
    }

    //JPEG
    else if(tmp[i]== 4){    
      length=calcul_length(tmp,i);
      //printf("\n Type : %d \n",tmp[i]);
      i=i+3;  
      //printf("Image JPEG :  length : %i  i= %d\n",length,tmp[i]);
      i=i+length+1;
      //printf("\n");    
    }


    //COMPOUND
    else if(tmp[i]== 5){
      //printf("\n Type : %d \n",tmp[i]);
      length=calcul_length(tmp,i);
      i=i+3;    
      //printf("TLV Compound :  length : %i  version: %d  \n",length,tmp[i+1]);
      length=length+i;
      load_daz(tmp,i+1,taille);
      i = i +length+1;
     // printf(".... %d \n",i);      
    }

    //DATE  
    else if(tmp[i]== 6){
   //   printf("\n Date Type : %d \n",tmp[i]);
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



 int load_daz1(unsigned char *tmp, int deb,int taille){
	
	dazibao * result ;
	result = (dazibao *) malloc (sizeof (dazibao));	
	int i=deb,cpt=0,cpt_pads=0;
	int j,k,length=0;
	//tlv * debut;
	//tlv * fin;
	char *val;
	time_t date;
	
	printf("\nTaille: %d  MBZ1\n ", taille); 
	while(i<taille){
		tlv * courant = (tlv*) malloc(sizeof(tlv));
		// printf("apres while i= %d taille = %d \n",i,taille);
		
		if(tmp[i]==0) {
		  printf("NB TLV pad1: %d\n",++cpt_pads); 
			courant->type = 0;
			courant->length = 0;
			//      printf("\n Version : %d \n",tmp[i]); 
			//      printf("Pad1\n"); 
			i++;
		}
		//tlv
		else if(tmp[i]== 1){
		  printf("NB TLV padN: %d\n",++cpt_pads);
			courant->type = 1;
			length = calcul_length(tmp, i);
			courant->length = length;
			//      printf("\n Version :");
			//      printf("TLV PadN :  length MBZ ");
			i=i+length;
		}
		
		//Text
		else if(tmp[i]== 2){
			printf("\nNB TLV: %d  i %d\n",++cpt,i);
			courant->type = 2;
			printf("Type : %d \n",tmp[i]);
			printf(" Text version \n");
			length=calcul_length(tmp,i);
			printf("tataille :  length : %d  %d\n",length,tmp[i]);
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
			printf(" --> %s \n",courant->value );
		}
		
		//PNG
		else if(tmp[i]== 3){    
		  printf("\nNB TLV: %d\n",++cpt);
			printf("Type : %d \n",tmp[i]);
			length=calcul_length(tmp,i);
			i=i+3;   
			printf("Image PNG :  length : %i  %d\n",length,tmp[i]);
			//length=length+i;
			i=i+length+1;
			//printf("\n");    
		}
		
		//JPEG
		else if(tmp[i]== 4){    
		  printf("\nNB TLV: %d\n",++cpt);
			length=calcul_length(tmp,i);
			printf("Type : %d \n",tmp[i]);
			i=i+3;  
			printf("Image JPEG :  length : %i  i= %d\n",length,tmp[i]);
			i=i+length+1;
			//printf("\n");    
		}
		
		
		//COMPOUND
		else if(tmp[i]== 5){
		  printf("\nNB TLV: %d\n",++cpt);
			printf("Type : %d \n",tmp[i]);
			length=calcul_length(tmp,i);
			i=i+3;    
			printf("TLV Compound :  length : %i  version: %d  \n",length,tmp[i+1]);
			length=length+i;
			
			
			load_daz1(tmp,i+1,taille);
			i = i +length+1;
			printf(".... %d END COMPOUND %d\n",i,cpt);      
		}
		
		//DATE  
		else if(tmp[i]== 6){
		  printf("\nNB TLV: %d\n",++cpt);
			printf("Version : %d \n",tmp[i]);
			printf("Date : ");
			//Calcul longueur 3 octets (i+3)
			length=calcul_length(tmp,i);
			i=i+3;
			//calcul date sur 4 octets i+4
			date = calcul_datebige(tmp,i);
			//Affichage DATE 
			affiche_date(date);
			i=i+4;
			length=length+i;
			load_daz1(tmp,i+1,length);
			i = i +length+1;
			printf(".... %d END DATE %d\n",i,cpt);  
		}
		
	}//fin_while(1)
	return cpt;
}



