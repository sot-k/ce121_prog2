#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <time.h>

#define MAX_NAME_SIZE 256
#define MAX_BUFF_SIZE 512

void check_argc(int flags);

void my_read_write(int fd_out, off_t file_size,int fd_in);
//Sinartisi gia tin anagnwsi kai to grapsimo ton periexomenon tou arxeiou
void check_error(int return_value, int line);//Sinartisi gia pithana error ton system calls

int main (int argc,char *argv[]) {

	int length,fd_in,return_value;
	char path[MAX_NAME_SIZE];//Buffer pou bazo to path
	char *delete=NULL;
	//Pointer pou briskei to teleutaio / sto path gia na paro to name tou arxeiou
	struct stat fileinfo;//Struct gia tin stat

	check_argc(argc);
	path[0] = '\0';//Bazo \0 stin arxi tou buffer gia na exo megedos 0
	scanf("%s",path);//diabazo path gia na gemisei o buffer
	while (strlen(path) != 0 ) {
		//Oso o buffer exei onoma arxeiou mesa diladi to megedos tou den einai 0
		if (stat(path,&fileinfo) == -1) {//An apotixei i stat
			fprintf(stderr,"%s : %d\n",strerror(errno),__LINE__);
			exit(0);
		}
		else {//An petixei i stat gemizei to struct tis
			delete = strrchr(path,'/');//Vrisko to teleutaio / sto path
			delete = delete + 1;
			//Proxorao ton pointer stin epomeni thesi pou arxizei to name tou arxeiou
			length = strlen(delete);//Perno to megethos tou name tou arxeiou
			return_value = write(STDOUT_FILENO,&length,sizeof(int));//Grafo megethos

			check_error(return_value,__LINE__);
			return_value = write(STDOUT_FILENO,delete,length);//Grafo onoma

			check_error(return_value,__LINE__);
			return_value = write(STDOUT_FILENO,&fileinfo.st_atim,sizeof(time_t));

			check_error(return_value,__LINE__);
			//Grafo ora teleutaias prosvasis
			return_value = write(STDOUT_FILENO,&fileinfo.st_mtim,sizeof(time_t));

			check_error(return_value,__LINE__);
			//Grafo ora teleutaias metavolis
			return_value = write(STDOUT_FILENO,&fileinfo.st_mode,sizeof(mode_t));

			check_error(return_value,__LINE__);
			//Grafo tipo arxeiou
			return_value = write(STDOUT_FILENO,&fileinfo.st_size,sizeof(off_t));

			check_error(return_value,__LINE__);
			//Grafo megethos arxeiou
			fd_in = open(path,O_RDWR);//Anoigo to arxeio
			check_error(fd_in,__LINE__);
			my_read_write(STDOUT_FILENO,fileinfo.st_size,fd_in);
			//diabazo kai grafo ta periexomena tou
		}
		path[0] = '\0';
		//bazo \0 gia na ginei 0 to megethos tou buffer
		scanf("%s",path);//diabazo epomeni eisodo (an uparxei)
	}
	exit(1);
}


void my_read_write(int fd_out, off_t file_size,int fd_in){

	char buffer[MAX_BUFF_SIZE];
	int return_value,fores,upoloipo,i;


	fores = ((int)file_size)/MAX_BUFF_SIZE;//Briskei posa 512 xorane
	upoloipo = ((int)file_size)%MAX_BUFF_SIZE;//Posa bytes menoun

	for(i=0;i<fores;i++) {
		return_value = read(fd_in,&buffer[0],MAX_BUFF_SIZE);
		//Diabazei apo to arxeio kai ta grafei ston buffer
    check_error(return_value,__LINE__);
		return_value = write(fd_out,&buffer[0],MAX_BUFF_SIZE);
		//Grafei ta periexomena tou pinaka
    check_error(return_value,__LINE__);
	}
	return_value=read(fd_in,&buffer[0],upoloipo);
  check_error(return_value,__LINE__);
	return_value = write(fd_out,&buffer[0],return_value);
  check_error(return_value,__LINE__);
}

void check_error(int return_value, int line){
  if (return_value < 0){
    fprintf(stderr,"%d : ERROR!", line);
    exit(0);
  }
}

void check_argc(int flags){
  if (flags<1){
    fprintf(stderr,"Too few arguments!\n");
    exit(0);
  }
  else if(flags>1){
    fprintf(stderr,"Too many arguments!\n");
    exit(0);
  }
}
