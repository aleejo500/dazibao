#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

typedef struct infos{
  char * path;
  int fd;
  time_t last_change;
  int connected;
}infos;

int memdate(time_t date1, time_t date2){
  struct tm * tm1 = localtime(&date1);
  struct tm * tm2 = localtime(&date2);
  printf("%02d:%02d:%02d:%02d:%02d:%02d  ",
	 tm1->tm_year+1900,tm1->tm_mon+1,tm1->tm_mday,
	 tm1->tm_hour,tm1->tm_min,tm1->tm_sec);
  printf("%02d:%02d:%02d:%02d:%02d:%02d\n",
	 tm2->tm_year+1900,tm2->tm_mon+1,tm2->tm_mday,
	 tm2->tm_hour,tm2->tm_min,tm2->tm_sec);
  if ( (tm1->tm_year+1900 == tm2->tm_year+1900) &&
       (tm1->tm_mon+1 == tm2->tm_mon+1) &&
       (tm1->tm_mday == tm2->tm_mday) &&
       (tm1->tm_hour == tm2->tm_hour) &&
       (tm1->tm_min == tm2->tm_min) &&
       (tm1->tm_sec == tm2->tm_sec))
    return 1;

  else 
    return 0;
}

/**
 * Ecrit la date dans le fichier pointé par le file descriptor fd à la ligne indiquée
 * format de la date : AAAAMMJJHHMMSS
 */
void write_date(int fd, int ligne, time_t date){
  struct tm * t = localtime(&date);
  char madate[15];
  snprintf(madate, 15, "%02d%02d%02d%02d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour, t->tm_min, t->tm_sec);
  lseek(fd,ligne*15+1, SEEK_SET);
  if ((write(fd,madate, 14)) < 0)
    perror("write");
  if ((write(fd,"\n", 1)) < 0)
    perror("write");
}

/**
 * Initialise notif.txt qui contient une date de dernière modif par fichier .dzb
 * les dates y sont triées dans le même ordre d'apparition que les dazibaos dans le tableau listes_infos
 */
infos * init(int nb_daz, char ** argv){
  int i,fd,rc,fl;
  infos * liste_infos = malloc(sizeof(infos)*(nb_daz));
  char path[BUFFER_SIZE];
  struct stat finfo;
  char buf[BUFFER_SIZE];

  strncpy(path, getenv("HOME"), BUFFER_SIZE);
  strncat(path, "/notif.txt", BUFFER_SIZE);
  if ((fd=open(path, O_RDWR|O_CREAT,0666)) < 0){
    perror("open notif.txt");
    exit(EXIT_FAILURE);
  }

  for (i=1; i< nb_daz+1; i++){
    infos inf;
    if ((rc = open(argv[i], O_RDONLY)) > 0){

      if ((fl=flock(rc,LOCK_EX)) < 0)
	perror("lock");

      if (fstat(rc, &finfo) < 0)
	perror("fstat");

      if ((fl=flock(rc,LOCK_UN)) < 0)
	perror("unlock");

      inf.connected = 0;

      printf("here 1\n");
      inf.path = getcwd(buf, BUFFER_SIZE);
      printf("here 2\n");
      
      printf("%s \n",buf);
      strncpy(inf.path, buf, BUFFER_SIZE);
      strncat(inf.path, "/", BUFFER_SIZE);
      strncat(inf.path, argv[i], BUFFER_SIZE);
      inf.fd = rc;
      inf.last_change = finfo.st_ctime;
      liste_infos[i-1] = inf;
      printf("%d\n",i-1);
      printf("%s\n",liste_infos[i-1].path);
      write_date(fd,i-1,inf.last_change);
      printf("%s\n",inf.path);
      close(rc);
    }
  }
  return liste_infos;
}

int main(int argc, char ** argv){
  struct sockaddr_un server;
  int server_fd,client_fd,pid,nb_daz;
  int i,fl,k,rc;
  infos inf;
  struct stat finfo;
  int clients[10], nb_clients,in;
  char buf[3];

  if (argc < 3){
    printf("Entrez au moins 2 dazibaos\n");
    printf("\tUsage : %s <dazibao1.dzb> <dazibao2.dzb> ... <dazibaoN.dzb>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("before init daz\n");
  nb_daz = argc-1;
  infos * liste_infos = init(argc-1, argv);
  //  for (k=0;k<nb_daz;k++)
    printf("%s\n", liste_infos[0].path);
    printf("%s\n", liste_infos[1].path);

  printf("just before socket init\n");
  memset(&server, 0, sizeof(server));
  if ((server_fd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
    perror("socket");
    exit(1);
  }

  server.sun_family = AF_UNIX;
  strncpy(server.sun_path, "", 103);
  strncat(server.sun_path, getenv("HOME"), 103);
  strncat(server.sun_path, "/", 107);
  strncat(server.sun_path, ".dazibao-notification-socket", 103);

  if (access(server.sun_path, F_OK) == 0){
    if (unlink(server.sun_path) != 0){
      perror("unlink");
      exit(1);
    }
  }

  if (bind(server_fd, (struct sockaddr*)&server, sizeof(struct sockaddr_un)) < 0){
    perror("bind");
    exit(1);
  }

  if (listen(server_fd, BUFFER_SIZE) < 0){
    perror("listen");
    exit(1);
  }

  while(1){

    if ((client_fd=accept(server_fd, NULL, NULL)) < 0){
      perror("accept");
      continue;
    }

    if ((pid=fork()) < 0){
      perror("fork");
      continue;
    }

    else if (pid == 0){
      while(1){
	printf("son\n");
	for (i=0; i< nb_daz; i++){
	  inf = liste_infos[i];
	  printf("%s\n", inf.path);
	  if ((rc=open(inf.path, O_RDWR)) < 0)
	    perror("open");
	  else
	    inf.fd = rc;
	  
	  if ((fl=flock(inf.fd, LOCK_EX)) < 0)
	    perror("lock");
	  
	  if (fstat(inf.fd, &finfo) < 0)
	    perror("fstat");
	  
	  if ((fl=flock(inf.fd,LOCK_UN)) < 0)
	    perror("unlock");
	  
	  close(rc);
	  
	  printf("compare\n");
	  if ((k=memdate(inf.last_change,finfo.st_ctime)) == 0){
	    printf("notif\n");//notif
	    continue;
	  } else if (k==1){
	    printf("no notif\n");
	  }
	}
	printf("end for\n");
	if ((in=read(STDIN_FILENO,buf,3)) <= 0){
	  perror("read");
	  continue;
	}
	printf("%c: %d", buf[0],(buf[0] == 'Q'));
	if (buf[0] == 'Q'){
	  printf("buf Q\n");
	  
	  for (k=0; k<nb_clients; k++)
	    close(clients[k]);
	  break;
	  return 0;
	}
      } 
    } else {
      printf("dad\n");
      if ((client_fd=accept(server_fd, NULL, NULL)) < 0){
	perror("accept");
	continue;
      }
      clients[nb_clients++] = client_fd;
      printf("read stop or not\n");
      if ((in=read(STDIN_FILENO,buf,3)) <= 0){
	perror("read");
	continue;
      }

      if (buf[0] == 'Q'){
	for (k=0; k<nb_clients; k++)
	  close(clients[k]);
	break;
      }
    }
  }

  return 0;
}
