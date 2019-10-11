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
#include "hw4_lib.h"
#include <sys/wait.h>


volatile sig_atomic_t clear_everythang = 0;

void ctrlc_handler() {
  clear_everythang = 7;
}

int main(int argc, char const *argv[]) {
    pid_t pid;
    struct sigaction act = { {0} };
    int shmid,memid,position,write_flag,read_flag;
    char *attached_mem, *reading_p, *writing_p,*pointerino_hack;
    char read_buf[MESSAGE_LEN],name[NAME_LEN];

    shmid = shmget(MEM_KEY,MEM_SIZE,IPC_CREAT | IPC_EXCL | S_IRWXU);
    if ((shmid == -1)&&(errno == EEXIST)) {
        memid = shmget(MEM_KEY,MEM_SIZE,0);
        attached_mem = (char *) shmat(memid,NULL,0);
        if ((long int)attached_mem == -1) {
          perror("shmat");
        }
        position = check_names(attached_mem,argv[1],argv[2]);
    }
    else if (shmid > 0){
        attached_mem = (char *) shmat(shmid,NULL,0);
        if ((long int)attached_mem == -1) {
          perror("shmat");
        }
        set_memo(attached_mem,argv[1],argv[2]);
        position = 1 ;
    }
    else{
      perror("shmget");
    }

    act.sa_handler = ctrlc_handler ;
    act.sa_flags = SA_RESTART ;
    sigaction(SIGINT,&act,NULL);

    sync_initialization();

    pid = fork();
    if (pid == 0) {
      //reader-child
      if (position == 1) {
        reading_p = (attached_mem + COUNTER_SIZE + 2 * (NAME_LEN) + MESSAGE_LEN);
        strcpy(name,attached_mem + COUNTER_SIZE + NAME_LEN + MESSAGE_LEN);
        read_flag = READ1_READY;
        write_flag = WRITE2_READY;
      }
      else{
        reading_p = (attached_mem + COUNTER_SIZE + NAME_LEN );
        strcpy(name,attached_mem + COUNTER_SIZE);
        read_flag = READ2_READY;
        write_flag = WRITE1_READY;
      }

      if (atoi(attached_mem) == 0) {
        strcpy(attached_mem,"1");
      }
      else {
        strcpy(attached_mem,"2");
      }

      while (1) {
        availability(read_flag);
        if (clear_everythang == 7) {
          break;
        }
        strcpy(read_buf,reading_p);
        printf("%s: %s\n",name,read_buf);
        completed(write_flag);
      }

    }
    else {
      //parent-writer
      if (position == 1) {
        writing_p = (attached_mem + COUNTER_SIZE + NAME_LEN );
        strcpy(name,attached_mem + COUNTER_SIZE);
        read_flag = READ2_READY;
        write_flag = WRITE1_READY;
      }
      else{
        writing_p = (attached_mem + COUNTER_SIZE + 2 * (NAME_LEN ) + MESSAGE_LEN);
        strcpy(name,attached_mem + COUNTER_SIZE + NAME_LEN + MESSAGE_LEN);
        read_flag = READ1_READY;
        write_flag = WRITE2_READY;
      }

      while (1) {
        fgets(read_buf,MESSAGE_LEN-1,stdin);
        if (strcmp(read_buf,"quit") == 0 || strcmp(read_buf,"Quit") == 0 || strcmp(read_buf,"QUIT") == 0 || clear_everythang == 7) {
          printf("IRTHAME AEK\n");

        }
        pointerino_hack = strchr(read_buf,'\n');
        strcpy(pointerino_hack,"");
        strcpy(writing_p,read_buf);
        completed(read_flag);
        availability(write_flag);
      }
    }

    return (0);
}
