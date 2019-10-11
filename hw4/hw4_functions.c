/*
	**************************
	*Karamellios Sotiris 2237*
	* Vassilis Samaras 2144  *
	**************************
*/

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
#include "hw4_lib.h"

void availability(int flag) {
  int msgqid;
  message_my_t m;

  msgqid = msgget(MEM_KEY,0);
  m.type = flag;
  msgrcv(msgqid,&m,2,flag,0);

}

void sync_initialization() {
    msgget(MEM_KEY,IPC_CREAT | IPC_EXCL | S_IRWXU);
}

void completed(int flag) {
  int msgqid;
  message_my_t m;

  msgqid = msgget(MEM_KEY,0);
  m.type = flag;
  strcpy(m.minima,"S");
  msgsnd(msgqid,&m,strlen(m.minima)+1,0);
}

void print_delimiter() {
    printf("================================================================================\n");
}

void set_memo(char *shmid,const char first_name[],const char second_name[]) {

    strcpy(shmid , "0");
    strcpy(shmid + COUNTER_SIZE,first_name);
    strcpy(shmid + COUNTER_SIZE + NAME_LEN + MESSAGE_LEN , second_name);

    return;
}

int check_names(char *shmid,const char first_name[],const char second_name[]){
    int position;

    if (strcmp(first_name,shmid + COUNTER_SIZE) == 0) {
      position = 1;
      if (strcmp(second_name,shmid + COUNTER_SIZE + NAME_LEN + MESSAGE_LEN ) == 0) {
        return(position);
      }
      else {
        printf("Expected %s and got %s!\n",shmid + COUNTER_SIZE + NAME_LEN + MESSAGE_LEN ,second_name);
        exit(0);
      }
    }
    else {
      if (strcmp (first_name,shmid + COUNTER_SIZE + NAME_LEN + MESSAGE_LEN ) == 0){
        position = 2;
        if (strcmp(second_name,shmid + COUNTER_SIZE) == 0) {
          return(position);
        }
        else {
          printf("Expected %s and got %s!\n",shmid + COUNTER_SIZE ,second_name);
          exit(0);
        }
      }
      else{
        printf("Forever alone!!!\n");
        exit(0);
      }
    }
}
