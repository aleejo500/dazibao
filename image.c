#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>

int main(){
  int fd, rc, lk;
  struct stat st;
  int buf_size;

  if ((fd=open("up.png", O_RDONLY)) < 0)
    perror("open");

  if ((lk=flock(fd,LOCK_EX)) < 0)
    perror("lock");

  if ((rc=fstat(fd, &st)) < 0)
    perror("stat");

  buf_size = st.st_size;
  char buf[buf_size];

  if ((rc=read(fd,buf,buf_size)) < 0)
    perror("read");

  if ((lk=flock(fd,LOCK_UN)) < 0)
    perror("unlock");

  if (close(fd) < 0)
    perror("close");

  if ((fd=open("up-copie.txt", O_WRONLY|O_CREAT, 0666)) < 0)
    perror("open | create");

  if ((lk=flock(fd,LOCK_EX)) < 0)
    perror("lock");

  if ((rc=write(fd, buf, buf_size)) < 0)
    perror("write");

  if ((lk=flock(fd,LOCK_UN)) < 0)
    perror("unlock");

  if (close(fd))
    perror("close");

  return 0;
}
