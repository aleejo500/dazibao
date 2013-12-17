//Fonction qui calcul la taille d'un TLV
int calcul_length (unsigned char *tmp,int i ){
  return ((256*256*tmp[i+1])+(256*tmp[i+2])+tmp[i+3]);
}

// MERYAM : time_t n int? pour l'affichage pt on utiliser directement les int?
time_t calcul_date(unsigned char *tmp,int i)
{
  return( (256*256*256*tmp[i+1])+(256*256*tmp[i+2])+(256*tmp[i+3])+tmp[i+4] );
}
// fonction qui convertit et affiche la date
void affiche_date(time_t date){
  struct tm *tlv_date;
  tlv_date = localtime(&date);
  printf("\t%d-%d-%d \n",tlv_date->tm_mday,tlv_date->tm_mon +1,1900+ tlv_date->tm_year);
}

void lect_tlv(unsigned char *tmp, int i,int taille){
  int length=0;
  time_t date;

  while(i<taille){
    // printf("apres while i= %d taille = %d \n",i,taille);
  
    if(tmp[i]==0) {
      printf("Version : %d \n",tmp[i]); 
      printf("Pad1\n"); 
      i++;
    }
    //tlv
    else if(tmp[i]== 1 ){
      printf("\nVersion : %d \n",tmp[i]);
      printf("TLV PadN : ");
      length=calcul_length(tmp,i);
      //length =longueur du MBZ +3 : nbre d'octets lu sur une longueur+1 passer au suivant
      i=i+length+3+1;
    }
  
    //Text
    else if(tmp[i]== 2){
      printf("\nVersion : %d \n",tmp[i]);
      printf("Text \n");
      length=calcul_length(tmp,i);
      i=i+3;
      length=length+i;
      // Boucle for qui affiche char by char tous le text
      for(i=i+1;i<=length;i++) printf("%c",tmp[i]);
      printf("\n");
    } 
  
    //PNG
    else if(tmp[i]== 3){    
      printf("\nVersion : %d \n",tmp[i]);
      printf("Image PNG \n");
      length=calcul_length(tmp,i);
      i=i+3;   
      printf("Image PNG :  length : %d\n",i-length);
      //Pas d'affichage on saute directement au suivant
      i=i+length+1;
      printf("\n");    
    }

    //JPEG
    else if(tmp[i]== 4){    
      printf("\nVersion : %d \n",tmp[i]);
      length=calcul_length(tmp,i);
      i=i+3;  
      printf("Image JPEG :  length : %d\n",i-length);
      i=i+length+1;
      printf("\n");    
    }


    //COMPOUND
    else if(tmp[i]== 5){
      printf("\nVersion : %d \n",tmp[i]);
      printf("Compound \n");
      length=calcul_length(tmp,i);
      i=i+3;    
      //printf("TLV Compound :  length : %i  version: %d  \n",length,tmp[i+1]);
      length=length+i;
      lect_tlv(tmp,i+1,length+1);
      i = i +length+1;
      printf("fin compound \n");
    }

    //DATE  
    else if(tmp[i]== 6){
      printf("\nVersion : %d \n",tmp[i]);
      printf("Date : ");
      //Calcul longueur 3 octets (i+3)
      length=calcul_length(tmp,i);
      i=i+3;
      //calcul date sur 4 octets i+4
      date = calcul_date(tmp,i);
      //Affichage DATE 
      affiche_date(date);
      i=i+4;
      length=length+i;
      lect_tlv(tmp,i+1,length);
      i = i +length+1;
    }
  
  }//fin_while(1)

}
