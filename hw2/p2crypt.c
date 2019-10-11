#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_BUFF_SIZE 256

void check_argc(int flags);
//Sinaritsi pou elenxei ton arithmo ton orismatwn apo to command line
void check_error(int return_value);
//Sinartisi pou elenxei ta return_values ton system calls gia pithana errors


int main(int argc,char *argv[]){

  int length,i,return_value,b;
  char buff_in[MAX_BUFF_SIZE],buff_out[MAX_BUFF_SIZE];

  check_argc(argc);

  length = strlen(argv[1]) ;//Diabazo to megedos tou kleidiou
  i = 0;
  while (1) {
    return_value = read(STDIN_FILENO,&buff_in[0],1);//Diabazei sto buff_in 1 byte
        check_error(return_value);
	if (i>= length){ //an kseperasw to megethos tou kleidiou arxizw apo tin arxi
		i = 0 ;
	}
    buff_out[0] = buff_in[0] ^ argv[1][i] ;
    // kanw xor ton xaraktira me ton antistoixo xaraktira tou kleidiou
    i++;
    b = write(STDOUT_FILENO,buff_out,1);//grafw to kriptografimeno byte
    check_error(b);
    if(return_value <= 0){//An kapia stimgi teleiosei to input tote stamatao
      exit(1);
    }
  }

}

void check_argc(int flags){
  if (flags<2){
    fprintf(stderr,"Too few arguments!\n");
    exit(0);
  }
  else if(flags>2){
    fprintf(stderr,"Too many arguments!\n");
    exit(0);
  }
}

void check_error(int return_value){
  if (return_value < 0){
    fprintf(stderr,"ERROR!");
    exit(0);
  }
}
