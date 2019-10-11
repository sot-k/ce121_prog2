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


#define MAX_NAME_SIZE 256
#define MAX_BUFF_SIZE 512

void check_error(int a, int line);
//Sinartisi pou elenxei ta pithana lathoi ton system calls
void check_argc(int flags);
//Sinaritsi pou elenxei ton arithmo ton parametron apo to command line
int my_read_write(int fd, off_t file_size);
//Sinartisi gia tin ektiposi tou periexomenou arxeiou


int main(int argc, char *argv[]) {

  int a,fd,name_size, read_bytes, bytes_copied;
  int bytes_to_copy,i;
  char file_name[MAX_NAME_SIZE];
  struct stat fileinfo;//Struct gia tin stat
  time_t last_access,last_modiffication;
  mode_t mode;
  off_t file_size;
  struct utimbuf timeinfo;//Struct gia tin utime


  check_argc(argc);

  a = mkdir(argv[1],0777);//Dimiourgo neo directory
  if (a == -1 ){//An iparksei provlima
    fprintf(stderr, "%s\n",strerror(errno) );
    exit(0);
  }
  a = chdir(argv[1]);//Alazo to directory sto opoio briskomai
  if (a == -1 ){//An den allaksei
    fprintf(stderr, "%s\n",strerror(errno) );
    exit(0);
  }
  read_bytes = 0;
  do {
    a = read(STDIN_FILENO,&name_size + read_bytes,sizeof(int) - read_bytes);
    check_error(a,__LINE__);
    read_bytes = read_bytes + a;
  } while(read_bytes < sizeof(int));

  //Diabazo to megethos tou onomatos tou arxeiou
  while (a > 0) {
    read_bytes = 0;
    do {
      a = read(STDIN_FILENO,&file_name[read_bytes],name_size - read_bytes);
      check_error(a,__LINE__);
      read_bytes = read_bytes + a;
    } while(read_bytes < name_size);
    //Diabazo to onoma tou arxeiou
	  file_name[name_size] = '\0' ;//Bazo sto telos \0
    fd = open(file_name,O_RDWR | O_CREAT,S_IRWXU);
    //Dimiourgo ena neo arxeio me to onoma pou diabasame
    check_error(fd,__LINE__);
    a = stat(file_name,&fileinfo);//Kanoume stat sto arxeio pou anoiskame
    if (a == -1){//AN apotixei i stat
      fprintf(stderr, "%s\n",strerror(errno) );
      exit(0);
    }
    read_bytes = 0;
	do {
      a = read(STDIN_FILENO,&last_access + read_bytes,sizeof(time_t) - read_bytes);
      check_error(a,__LINE__);
      read_bytes = read_bytes + a;
    } while(read_bytes < sizeof(time_t));

    //Diabazo tin ora teleutaias prospelasis tou paliou arxeiou
    timeinfo.actime = last_access ;
    //Allazo to periexomeno tou struct tis utime gia tin teleutaia prosbasi
    read_bytes = 0;
    do {
      a = read(STDIN_FILENO,&last_modiffication + read_bytes,sizeof(time_t) - read_bytes);
      check_error(a,__LINE__);
      read_bytes = read_bytes + a;
    } while(read_bytes < sizeof(time_t));

  //Diabazo tin ora teleutaias metavolis tou paliou arxeiou
    timeinfo.modtime = last_modiffication ;
    //Allazo to periexomeno tou struct tis utime gia tin teleutaia metaboli
    read_bytes = 0;
    do {
      a = read(STDIN_FILENO,&mode + read_bytes,sizeof(mode_t) - read_bytes);
      check_error(a,__LINE__);
      read_bytes = read_bytes + a;
    } while(read_bytes < sizeof(mode_t));

    //Diabazo ton tipo tou paliou arxeiou
	  a = chmod(file_name,mode);//Allazo to mode tou arxeiou
    check_error(a,__LINE__);
    read_bytes = 0;
    do {
      a = read(STDIN_FILENO,&file_size + read_bytes,sizeof(off_t) - read_bytes);
      check_error(a,__LINE__);

      read_bytes = read_bytes + a;
    } while(read_bytes < sizeof(off_t));

    //Diabazo to megethos tou paliou arxeiou

    bytes_to_copy = (int)file_size ;
    while(bytes_to_copy > 0){
      bytes_copied = my_read_write(fd,bytes_to_copy);

      bytes_to_copy = bytes_to_copy - bytes_copied ;
    }

    //Grafo sto neo to periexomeno tou paliou arxeiou
	  a = utime(file_name,&timeinfo);
    //Bazo ta alagmena periexomena tou struct tis utime sto arxeio
    check_error(a,__LINE__);

    close(fd);//Kleino to arxeio pou anoiksa
    read_bytes = 0;
	i = 0;
    do {
      a = read(STDIN_FILENO,&name_size + read_bytes,sizeof(int) - read_bytes);
      check_error(a,__LINE__);
	  i++;
      if (i>10){
        break;
      }
      //fprintf(stderr, "%d\n",read_bytes );
      read_bytes = read_bytes + a;
    } while(read_bytes < sizeof(int));
    //Den ksexnao na do mipos exoume kai alli eisodo
  }
  exit(1);
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

void check_error(int a, int line){
  if (a < 0){
    fprintf(stderr,"%d ERROR!\n", line);
    exit(0);
  }
}

int my_read_write(int fd, off_t file_size){

  char buffer[MAX_BUFF_SIZE];
	int a;

  if(file_size > MAX_BUFF_SIZE){
    file_size = MAX_BUFF_SIZE ;
  }

	a = read(STDIN_FILENO,&buffer[0],file_size);
  check_error(a,__LINE__);
	a = write(fd,&buffer[0],a);
  check_error(a,__LINE__);

  return(a);
}
