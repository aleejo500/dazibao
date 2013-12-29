int add_pad_n(int fd, int taille, int indice,int type){
  int fl;
  char * zero[taille];
  memset(&zero, 0, taille);

  if ((fl=flock(fd, LOCK_EX)) < 0)
    perror("add_pad_n function, lock");
  
  lseek(fd,indice,SEEK_SET);
  if ((write(fd,&type,1)) < 0)
    perror("add_pad_n function, write TLV's type");

  lseek(fd,3,SEEK_CUR);
  if ((write(fd,&zero, taille)) < 0)
    perror("add_pad_n function, write TLV's size");

  if ((fl=flock(fd, LOCK_UN)) < 0)
    perror("add_pad_n function, unlock");

  return 0;
}

int add_tlv_txt1(dazibao *dazchargee,int type,int fd,int taille,int dated){
  tlv *nuevotlv,*courant;
  int l=140,lk;
  unsigned char buffer[l];
  int bytes,newsize;
  unsigned int now=(unsigned int)time(NULL);
	
  if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
    errno=EAGAIN;
    return -1;
  }
	
  courant=dazchargee->tlv_fin;
  bytes=read(STDIN_FILENO,buffer,l);
  if (bytes<0)
    perror("add_tlv_txt1 function, read from stdout");
	
  if ((nuevotlv->value = (unsigned char *) malloc (l * sizeof(unsigned char))) == NULL) {
    errno=EAGAIN;
    return -1;
  }	

  newsize=bytes+taille+5;
  if (dated==6) {
    printf("Timestamp: %d\n",now);
    newsize+=8;		
  }

  nuevotlv=creer_tlv(type,bytes,buffer);
  nuevotlv->suivant = NULL; 

  dazchargee->tlv_fin = nuevotlv; 
  dazchargee->size+=nuevotlv->length; 
	
  if ((lk=flock(fd,LOCK_EX)) < 0)
    perror("add_tlv_txt1 function, lock");
	
  if (ftruncate(fd,newsize) < 0)
    perror("add_tlv_txt1 function, ftruncate");
	
  lseek(fd,taille ,SEEK_SET);
  if (dated==6) {
    if((write(fd,&dated,1))<=0) {
      errno=EFAULT;
      perror("add_tlv_txt1 function, write");
      close(fd);	
      return -1;
    }
    lseek(fd, 2, SEEK_CUR);
    if((write(fd,&bytes+8,2))<=0) {
      errno=EFAULT;;
      perror("add_tlv_txt1 function, write TLV's size");
      close(fd);	
      return -1;
    }
    lseek(fd, -1, SEEK_CUR);
    if((write(fd,&now,4))<=0) {
      errno=EFAULT;
      perror("add_tlv_txt1 function, write dated");
      close(fd);	
      return -1;
    }
  }
  if((write(fd,&nuevotlv->type,1))<=0) {
    errno=EFAULT;
    perror("add_tlv_txt1 function, write TLV's type");
    close(fd);	
    return -1;
  }
  lseek(fd, 0, SEEK_CUR);
  if((write(fd,&bytes,3))<=0) {
    errno=EFAULT;;
    perror("add_tlv_txt1 function, write TLV's size");
    close(fd);	
    return -1;
  }

  lseek(fd,-bytes-1,SEEK_END);
  if((write(fd,&buffer,nuevotlv->length))<=0) {
    errno=EFAULT;
    perror("add_tlv_txt1 function, write TLV's value");
    close(fd);	
    return -1;
  }

  if ((lk=flock(fd,LOCK_UN)) < 0)
    perror("add_tlv_txt1 function, unlock");
	
  if (close(fd)<0)
    perror("add_tlv_txt1 function, close");
	
  return bytes;
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
  if ((fd=open(buffer,O_RDONLY,0666)) < 0){
    perror("add_tlv_picture function, open");
    free(buffer);
    return -1;
  }	

  if ((lk=flock(fd,LOCK_EX)) < 0)
    perror("add_tlv_picture function, lock");
		
  if ((rc=fstat(fd, &st)) < 0)
    perror("add_tlv_picture function, fstat");
		
  buf_size = st.st_size;
  unsigned char buf[buf_size];
  if ((rc=read(fd,buf,buf_size)) < 0)
    perror("add_tlv_picture function, read");
		
  if ((lk=flock(fd,LOCK_UN)) < 0)
    perror("add_tlv_picture function, unlock");
		
  if (close(fd) < 0)
    perror("close");

  if ((nuevotlv->value = (unsigned char *) malloc (buf_size * sizeof (unsigned char))) == NULL) 
    return -1; 
	
  newsize=buf_size+taille+5;	
  if (dated==6) {
    printf("Timestamp: %d\n",now);
    newsize+=8;
  }
  nuevotlv=creer_tlv(type,buf_size,buf);
  nuevotlv->suivant = NULL; 

  dazchargee->tlv_fin = nuevotlv; 
  dazchargee->size+=nuevotlv->length; 
	
  if ((lk=flock(fd_daz,LOCK_EX)) < 0)
    perror("add_tlv_picture function, lock daz");
	
  if (ftruncate(fd_daz,newsize) < 0)
    perror("Truncate daz error");
	
  lseek(fd_daz,taille ,SEEK_SET);	
  if (dated==6) {
    if((write(fd_daz,&dated,1))<=0) {
      errno=EFAULT;
      perror("add_tlv_picture function, write TLV's type");
      close(fd_daz);	
      return -1;
    }

    lseek(fd_daz, 0, SEEK_CUR);
    if((write(fd_daz,&nuevotlv->length,3))<=0) {
      errno=EFAULT;
      perror("add_tlv_picture function, write TLV's size");
      close(fd_daz);	
      return -1;
    }	
    lseek(fd_daz, 0, SEEK_CUR);
    if((write(fd_daz,&now,2))<=0) {
      errno=EFAULT;;
      perror("add_tlv_picture function, write dated");
      close(fd_daz);	
      return -1;
    }
  }
  if((write(fd_daz,&nuevotlv->type,1))<=0) {
    errno=EFAULT;
    perror("add_tlv_picture function, write TLV's type");
    close(fd_daz);	
    return -1;
  }	

  lseek(fd_daz, 0, SEEK_CUR);
  if((write(fd_daz,&nuevotlv->length,3))<=0) {
    errno=EFAULT;
    perror("add_tlv_picture function, write TLV's size");
    close(fd_daz);	
    return -1;
  }

  lseek(fd_daz,-nuevotlv->length-1,SEEK_END);	
  if((write(fd_daz,&buf,buf_size))<=0) {
    errno=EFAULT;
    perror("add_tlv_picture function, write TLV's value");
    close(fd_daz);	
    return -1;
  }

  if ((lk=flock(fd_daz,LOCK_UN)) < 0)
    perror("add_tlv_picture function, unlock");
	
  if (close(fd_daz)<0)
    perror("add_tlv_picture function, close");

  return 1;
}

int add_tlv_compound(dazibao *dazchargee,int type,int fd,int taille,int flag){	
  tlv *nuevotlv,*courant;
  int bytes,newsize,lk;
	
  if ((nuevotlv = (tlv *) malloc (sizeof (tlv))) == NULL) {
    errno=EAGAIN;
    return -1;
  }
  courant=dazchargee->tlv_fin;
  bytes=0;	

  newsize=taille+4;
  nuevotlv=creer_tlv(type,bytes,NULL);
  nuevotlv->suivant = NULL; 

  dazchargee->tlv_fin = nuevotlv; 
  dazchargee->size+=nuevotlv->length; 

  if ((lk=flock(fd,LOCK_EX)) < 0)
    perror("add_tlv_compound function, lock");
	
  if (ftruncate(fd,newsize) < 0)
    perror("add_tlv_compound function, truncate daz");
	
  lseek(fd,taille ,SEEK_SET);
  if((write(fd,&nuevotlv->type,1))<=0) {
    errno=EFAULT;
    perror("add_tlv_compound function, write TLV's type");
    close(fd);	
    return -1;
  }

  if((write(fd,&bytes,3))<=0) {
    errno=EFAULT;;
    perror("add_tlv_compound function, write TLV's size");
    close(fd);	
    return -1;
  }
		
  if ((lk=flock(fd,LOCK_UN)) < 0)
    perror("add_tlv_compound function, unlock");
	
  return newsize;
}

int del(char * path,int delete_index){// fonction de suppression
  int fd,fl,rd,taille,tlv_taille;
  int cpt=4, delete_cpt=0;
  struct stat finfo;
  unsigned char * buf;	

  if ((fd=open(path, O_RDWR)) < 0){
    perror("del funtion, open");
  }
	
  if ((fl=flock(fd, LOCK_EX)) < 0){
    perror("del funtion, lock");
  }
	
  if(fstat(fd,&finfo)< 0)
    perror("del funtion, fstat");	
	
  taille= finfo.st_size;
  buf = malloc(sizeof(char)*taille);
  if((rd = read(fd,buf,taille)) < 0 ){
    errno=EACCES;
    perror("del funtion, read");
  }

  if ((fl=flock(fd, LOCK_UN)) < 0){
    perror("del funtion, unlock");
  }  
	
  while (cpt <= taille){
    if ((buf[cpt] >1) && (buf[cpt] < 7)){
      delete_cpt ++;
      cpt+=4;
    }
    if (delete_cpt == delete_index){
      cpt-=4;
      break;
    }  
    cpt++;  
  }
  tlv_taille = calcul_length_litt(buf,cpt);
  add_pad_n(fd, tlv_taille,cpt,1);
  close(fd);
  return 0;
}
