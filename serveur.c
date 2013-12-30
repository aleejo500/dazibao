#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 20
char * notif_path;

void write_date(int fd, int ligne, time_t date){
  struct tm * t = localtime(&date);
  char madate[15];
  snprintf(madate, 15, "%02d%02d%02d%02d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour, t->tm_min, t->tm_sec);
  lseek(fd,ligne*15+1, SEEK_SET);
  if ((write(fd,madate, 14)) < 0)
    perror("write_date function, write");
  if ((write(fd,"\n", 1)) < 0)
    perror("write_date function, write");
}

int memdate(int ligne, time_t date){
  int fd,rc,i;
  char buf1[15], buf2[15];
  struct tm * tm = localtime(&date);

  if ((fd=open(notif_path, O_RDWR)) < 0){
    perror("memdate function, open $HOME/notif.txt");
    return 0;
  }
  if (lseek(fd,ligne*15+1, SEEK_SET) < 0){
    perror("memdate function, lseek");
    return 0;
  }
  if ((rc=read(fd, buf1, 14)) < 0){
    perror("memdate function, read");
    return 0;
  }
  snprintf(buf2, 15,"%02d%02d%02d%02d%02d%02d", tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
  for (i=0; i<14; i++){
    if (!(buf1[i]==buf2[i])){
      write_date(fd,ligne,date);
      return 0;
    }
  }
  close(fd);
  return 1;
}

char ** init(int nb_daz, char ** argv){
  int i,fd,rc,fl;
  char ** daz_paths = malloc(sizeof(char *)*nb_daz);
  struct stat finfo;

  notif_path = malloc(sizeof(char) * BUFFER_SIZE);
  strncpy(notif_path, getenv("HOME"), BUFFER_SIZE);
  strncat(notif_path, "/notif.txt", BUFFER_SIZE);
  if ((fd=open(notif_path, O_WRONLY|O_CREAT,0666)) < 0){
    perror("init function, open");
    exit(EXIT_FAILURE);
  }

  for (i=1; i< nb_daz+1; i++){
    char * buf = malloc(sizeof(char)*BUFFER_SIZE);
    if ((rc = open(argv[i], O_RDONLY)) > 0){

      if ((fl=flock(rc,LOCK_EX)) < 0)
	perror("init function, lock");

      if (fstat(rc, &finfo) < 0)
	perror("init function, fstat");

      if ((fl=flock(rc,LOCK_UN)) < 0)
	perror("init function, unlock");

      getcwd(buf, BUFFER_SIZE);
      strncat(buf, "/", BUFFER_SIZE);
      strncat(buf, argv[i], BUFFER_SIZE);
      write_date(fd,i-1,finfo.st_ctime);
      daz_paths[i-1] = buf;
      close(rc);
    }
  }
  return daz_paths;
}

void notif(int * clients, char * path){
  int length=0,wr,i;
  while(path[length] != '\0')
    length++;
  for (i=0; i<MAX_CLIENTS; i++){
    if (clients[i] != 0){
      if ((wr=write(clients[i], path, length)) < 0)
	perror("notif function, read");
    }
  }
}

int main(int argc, char ** argv){
  struct sockaddr_un server;
  int server_fd,client_fd,pid,nb_daz;
  int i,fl,k,rc;
  struct stat finfo;
  int nb_clients=0;
  int * clients = malloc(sizeof(int)*MAX_CLIENTS);
  char buf[3];
  char ** daz_paths;

  if (argc < 3){
    printf("Entrez au moins 2 dazibaos\n");
    printf("\tUsage : %s daz1.dzb daz2.dzb <daz3.dzb> ... <dazN.dzb>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  nb_daz = argc-1;
  daz_paths = init(argc-1, argv);
  printf("\nDazibaos :\n");
  for (k=0;k<nb_daz;k++)
    printf("  %s\n", daz_paths[k]);

  memset(&server, 0, sizeof(server));
  if ((server_fd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
    perror("server main, socket");
    exit(1);
  }

  server.sun_family = AF_UNIX;
  strncpy(server.sun_path, getenv("HOME"), 107);
  strncat(server.sun_path, "/.dazibao-notification-socket", 107);

  if (access(server.sun_path, F_OK) == 0){
    if (unlink(server.sun_path) != 0){
      perror("server main, unlink");
      exit(1);
    }
  }

  if (bind(server_fd, (struct sockaddr*)&server, sizeof(struct sockaddr_un)) < 0){
    perror("server main, bind");
    exit(1);
  }

  if (listen(server_fd, BUFFER_SIZE) < 0){
    perror("server main, listen");
    exit(1);
  }

  while(1){
    if ((client_fd=accept(server_fd, NULL, NULL)) < 0){
      perror("server main, accept");
      continue;
    }
    clients[nb_clients++] = client_fd;

    if ((pid=fork()) < 0){
      perror("server main, fork");
      continue;
    }

    else if (pid == 0){
      while(1){
	for (i=0; i< nb_daz; i++){
	  if ((rc=open(daz_paths[i], O_RDONLY)) < 0){
	    printf("%d ", i);
	    perror("server main, open");
	    continue;
	  }

	  if ((fl=flock(rc, LOCK_EX)) < 0)
	    perror("server main, lock");
	  
	  if (fstat(rc, &finfo) < 0)
	    perror("server main, fstat");
	  
	  if ((fl=flock(rc,LOCK_UN)) < 0)
	    perror("server main, unlock");
	  
	  close(rc);
	  if ((k=memdate(i,finfo.st_ctime)) == 0){
	    notif(clients,daz_paths[i]);
	    printf("\n %s has been changed", daz_paths[i]);
	    break;
	  } else if (k==1){
	    continue;
	  }
	}
      } 
    }
    
    else {
      if (buf[0] == 'Q'){
	for (k=0; k<nb_clients; k++){
	  close(clients[k]);
	}
	break;
      } else {
	continue;
      }
    }
  }
  close(server_fd);
  return 0;
}
