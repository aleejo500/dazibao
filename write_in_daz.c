
int write_tlv(unsigned char *tmp,int taille,tlv *le_tlv,int fd){
	
	int i=taille;
	int new_taille;		
	int j,k,length=0;
	new_taille=taille+le_tlv->length;

	printf("TLV length:%d  val MBZ= %s , newt: %d ", taille,le_tlv->value, new_taille); 
  
	//while(i<new_taille){ //ecrire type
	
	if (ftruncate(fd,new_taille) < 0)
		perror("Truncate daz error");
	
		if((write(fd,le_tlv->value,le_tlv->length))<=0) {
		errno=EAGAIN;
		perror("write error");
		close(fd);	
		return -1;
	}
	
	
	//i++;
	//}//fin_while(1)
	close(fd);
	return 1; 
}
