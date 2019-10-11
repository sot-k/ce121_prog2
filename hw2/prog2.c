#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utime.h>
#include <sys/wait.h>


#define MAGIC_NUMBER_SIZE 9

void check_argc(int flags);

int is_base(int *fd, char *name);

void check_error(int a);

int main(int argc, char *argv[]) {

  int fd, i;
  pid_t dirlistpid,p2archivepid,p2cryptpid,p2unarchivepid;
  int status,fd_pipe[2],fd_pipe2[2];

  check_argc(argc);
/* kanw elegxous gia to -E/-D kai gia tin iparksi tou arxeiou/an einai swsto
i oxi*/


  if (strcmp("-E",argv[1]) == 0){
    fd = open (argv[4], O_RDWR|O_CREAT|O_EXCL, S_IRWXU);
    if (fd == -1 && errno == EEXIST){
      printf("File already exists!\n");
      close(fd);
      exit(0);
    }
    else if(fd == -1){
      check_error(fd);
    }
    else {
      write(fd,"P2CRYPTAR",MAGIC_NUMBER_SIZE);
    }

    pipe(fd_pipe); //dimiourgw ta pipes
    pipe(fd_pipe2);

    dirlistpid = fork();

    if (dirlistpid == -1){
      perror("fork");
    }
    else if(dirlistpid == 0){
		close(fd_pipe2[1]);//kleinw osa den xreiazontai
		close(fd_pipe2[0]);
      close(fd_pipe[0]);
      dup2(fd_pipe[1],STDOUT_FILENO);
      close(fd_pipe[1]);
      execlp("./dirlist","dirlist",argv[2],NULL);
    }
    p2archivepid = fork();

    if (p2archivepid == -1){
      perror("fork");
    }
    else if(p2archivepid == 0){
      close(fd_pipe[1]);
      close(fd_pipe2[0]);
      dup2(fd_pipe[0],STDIN_FILENO);
      dup2(fd_pipe2[1],STDOUT_FILENO);
      close(fd_pipe[0]);
      close(fd_pipe2[1]);

      execlp("./p2archive","p2archive",NULL);
    }
    close(fd_pipe[0]);
    close(fd_pipe[1]);
    p2cryptpid = fork();

    if (p2cryptpid == -1){
      perror("fork");
    }
    else if(p2cryptpid == 0){
      close(fd_pipe2[1]);
      dup2(fd_pipe2[0],STDIN_FILENO);
      dup2(fd,STDOUT_FILENO);
      close(fd_pipe2[0]);

      execlp("./p2crypt","p2crypt",argv[3],NULL);
    }
    close(fd_pipe2[0]);
    close(fd_pipe2[1]);
    for (i = 0; i < 3; i++) {//perimenw na teleiwsoun oi diergasies kai elegxw
      //an epestrepsan xwris problima i an eixan kapoio problima
      waitpid(-1, &status, 0);
      if(WIFEXITED(status)){
        if(WEXITSTATUS(status) == 0){
          printf("AN ERROR HAS OCCURED!\n");
          exit(0);
        }
      }
    }

    close(fd);
  }




  else if(strcmp("-D",argv[1])== 0){
    fd = open (argv[4], O_RDWR|O_CREAT|O_EXCL, S_IRWXU);
    is_base(&fd,argv[4]);

    pipe(fd_pipe);

    p2cryptpid = fork();

    if (p2cryptpid == -1){
      perror("fork");
    }
    else if(p2cryptpid == 0){
      close(fd_pipe[0]);
      dup2(fd,STDIN_FILENO);
      dup2(fd_pipe[1],STDOUT_FILENO);
      close(fd_pipe[1]);

      execlp("./p2crypt","p2crypt",argv[3],NULL);
    }

    p2unarchivepid = fork();

    if (p2unarchivepid == -1){
      perror("fork");
    }
    else if(p2unarchivepid == 0){
      close(fd_pipe[1]);
      dup2(fd_pipe[0],STDIN_FILENO);
      close(fd_pipe[0]);
      //sleep(2);
      execlp("./p2unarchive","p2unarchive",argv[2],NULL);
    }
    close(fd_pipe[0]);
    close(fd_pipe[1]);
    for (i = 0; i < 2; i++) {
      waitpid(-1, &status, 0);
      if(WIFEXITED(status)){
        if(WEXITSTATUS(status) == 0){
          printf("AN ERROR HAS OCCURED!\n");
          exit(0);
        }
      }
    }

    close(fd);
  }
  else{
    printf("Argument 1 is wrong. Try \"-E\" or \"-D\"!\n");
  }




  return (0);
}

void check_argc(int flags){
  if (flags<5){
    fprintf(stderr,"Too few arguments!\n");
    exit(0);
  }
  else if(flags>5){
    fprintf(stderr,"Too many arguments!\n");
    exit(0);
  }
}


int is_base (int *fd,char *name) {

	char temp_array[MAGIC_NUMBER_SIZE],magic_number[] = {"P2CRYPTAR"};
  /*Ston temp_array grafoume apo tin basi kai ston
  magic_number grafoume oti exei to arxeio elenxou*/
	int a;

	if (*fd == -1 && errno == EEXIST) {
    //An iparxei idi basi elegxw an einai diki mou
		a = *fd = open(name,O_RDWR);
		check_error(a);
		a = read(*fd,temp_array,MAGIC_NUMBER_SIZE);
		check_error(a);
		if ((memcmp(magic_number,temp_array, MAGIC_NUMBER_SIZE)) == 0) {
			return(1);
		}
		else {
			printf ("Wrong file input!\n");
		}
	}
	else if (*fd != -1 ) {
    printf("File does not exist!\n");
  }
  else {
    check_error(*fd);
  }
  close(*fd);
  exit(0);
}

void check_error(int a){
  if (a < 0){
    fprintf(stderr,"ERROR!");
    exit(0);
  }
}
