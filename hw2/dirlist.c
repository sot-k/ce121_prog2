#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


#define MAX_NAME_SIZE 256

void check_argc(int flags);

int main (int argc, char *argv[]) {
 
	DIR *dir;//Pointer gia to directory pou tha anoiksoume
	struct dirent *dp;//Aparaitito struct gia tin readdir
	struct stat sb;//Aparaitito struct gia tin stat
	char tempname[MAX_NAME_SIZE];//Pinakas pou bazoume to onoma
	int length;

	check_argc(argc);
	//Sinartisi pou elenxei ton arithmo ton orismaton apo to command line

	dir = opendir(argv[1]);//Anoigoume to directory pou theloume
	if(dir == NULL){//Se periptosi pou apotixei i opendir
		fprintf(stderr,"%s : %d\n",strerror(errno),__LINE__);
		exit(0);
	}
	errno = 0;
	do{
		if((dp = readdir(dir))!=NULL){//An petixei i readdir
			strcpy(&tempname[0],argv[1]);//Antigrafoume to path apo to command line
			length=strlen(argv[1]);//Vriskoume to megethos tou path
			strcpy(&tempname[length],"/");//Bazoume xeirokinita to /
			strcpy(&tempname[length+1],dp->d_name);
			//Antigrafoume to name tou arxeiou mesa sto path
			if(stat(tempname,&sb)==-1){//An apotixei i stat na anoiksei to arxeio
				fprintf(stderr,"%s : %d\n",strerror(errno),__LINE__);
			}
			else if((sb.st_mode & S_IFMT)==S_IFREG){
				//An to arxeio theloume na to emfanisoume
				printf("%s/%s\n",argv[1],dp->d_name);
			}
		}
		else if(errno<0){//An apotixei i readdir
			fprintf(stderr,"%s : %d\n",strerror(errno),__LINE__);
			exit(0);
		}
	}while (dp!=NULL);//An den exei na diabasei allo i readdir

	closedir(dir);
	//Den ksexname na kleisoume ton pointer se directory pou anoiksame
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
