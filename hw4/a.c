
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/msg.h>

//#include "hw4_lib.h"
#include <sys/wait.h>


struct message{
    long type;
    char minima[100];
}typedef struct message message_my_t;
#define ping 1
#define pong 2





int main(int argc, char const *argv[]) {

struct message m;
int msqid;
pid_t pid;

msqid = msgget(9000 , IPC_CREAT | S_IRWXU);
pid = fork();
if (pid == 0) {
  m.type = pong;
  strcpy(m.minima,"λσδαξηδφξηξακδηξγηβφξκα");
  msgsnd(msqid,&m,strlen(m.minima)+1,0 );
  msgrcv(msqid,&m,100,ping,0);
  printf("i got %s\n", m.minima);
}
else{
  msgrcv(msqid,&m,100,pong,0);
  m.type = ping ;
  printf("got  %s\n",m.minima );
  scanf(" %s", &m.minima);
  strcpy(m.minima+strlen(m.minima),"");
  msgsnd(msqid,&m,strlen(m.minima)+1,0);
}

  return 0;
}
