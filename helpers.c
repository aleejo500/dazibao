#include <stdio.h>
#include <stdlib.h>

void err_sys(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
void err_champ(char *msg){
  printf("%s",msg);
  exit(1);
}
