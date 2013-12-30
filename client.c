#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUFFER_SIZE 1024

int main()
{
    char buf[BUFFER_SIZE];
    struct sockaddr_un sun;
    int fd, bufptr, rc;

    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_UNIX;
    // strncpy(sun.sun_path, "", 103);
    strncpy(sun.sun_path, getenv("HOME"), 103);
    strncat(sun.sun_path, "/.dazibao-notification-socket", 107);

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        perror("socket");
        exit(1);
    }

    rc = connect(fd, (struct sockaddr*)&sun, sizeof(sun));
    if(rc < 0) {
        perror("connect");
        exit(1);
    }

    bufptr = 0;
    while(1) {
      if ((rc = read(fd, buf, BUFFER_SIZE)) < 0){
	if (errno == EINTR)
	  continue;

	perror("notif main, read");
	exit(1);
      }

      else if (rc == 0)
	break; //connexion interrompue

      else
	printf("Path of the changed dazibao:\n\t%s\n",buf);
    }
    return 0;
}
