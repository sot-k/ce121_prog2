#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define MAGIC_NUMBER_SIZE 11
#define MAX_STR_SIZE 256
#define BUFFERSIZE 512

int is_base (int *fd,char *name);
int myimport(int fd,char *file_name);
int mysearch(int fd);
int myexport(int fd);
int mydelete(int fd,char *from,char *what);
void myerror(int error,int line);
void my_read_write(int fd_source,int file_size,int fd_destination);


int main (int argc, char *argv[]) {
	
	int  fd;
	char answer, name[MAX_STR_SIZE];
	
	if(argc<2){
		printf("Error. Too few arguments\n");
		exit(0);
	}
	else if(argc>2){
		printf("Error. Too many arguments\n");
		exit(0);
	}
	
	if(strcmp(argv[1],"TextFile\0")==0){
		printf("Invalid Database\n");
		exit(0);
	}
	
	fd = open (argv[1], O_RDWR|O_CREAT|O_EXCL, S_IRWXU);
	is_base(&fd,argv[1]);
	do {
		printf ("Select Action\n1.Import\n2.Search\n3.Export\n4.Delete\n5.Exit\n");
		answer = getchar();
		switch (answer) {
			case '1': {
				myimport(fd,argv[1]);
				break;
			}
			case '2': {
				mysearch(fd);
				break;
			}
			case '3': {
				myexport(fd);
				break;
			}
			case '4': {
				printf("Enter file name to be deleted\n");
				getchar();
				fgets(name,MAX_STR_SIZE,stdin);
				mydelete(fd,argv[1],name);
				break;
			}
			case '5': {break;}
			default : {
			printf ("Invalid Option\n");
			}
		}
	}while (answer != '5');
	close (fd);
	return(0);
}


int is_base (int *fd,char *name) {
	
	char temp_array[MAGIC_NUMBER_SIZE],magic_number[MAGIC_NUMBER_SIZE];//Ston temp_array grafoume apo tin basi kai ston magic_number grafoume oti exei to arxeio elenxou
	int fd_magic_number,possible_error;
	if (*fd == -1 && errno == EEXIST) {//An iparxei idi basi elegxw an einai diki mou
		possible_error=*fd = open(name,O_RDWR);
		myerror(possible_error,__LINE__);
		possible_error=fd_magic_number = open ("TextFile", O_RDONLY);
		myerror(possible_error,__LINE__);
		possible_error=read(*fd,&temp_array[0],MAGIC_NUMBER_SIZE);
		myerror(possible_error,__LINE__);
		possible_error=read(fd_magic_number,&magic_number[0],MAGIC_NUMBER_SIZE);
		myerror(possible_error,__LINE__);
		close(fd_magic_number);
		if (memcmp(temp_array,magic_number,MAGIC_NUMBER_SIZE)==0) {
			return(1);
		}
		else {
			printf ("Invalid Database\n");
			exit(0);
		}
	}
	else if (*fd != -1 ) {//An den iparxei dimiourgw to arxeio kai tou prosthetw to magic_number
		fd_magic_number = open ("TextFile", O_RDONLY);
		myerror(fd_magic_number,__LINE__);
		possible_error=read(fd_magic_number,magic_number,MAGIC_NUMBER_SIZE);
		myerror(possible_error,__LINE__);
		close(fd_magic_number);
		possible_error=write(*fd,&magic_number[0],MAGIC_NUMBER_SIZE);
		myerror(possible_error,__LINE__);
		return(1);
	}
	else {
		printf("Unexpected error");
		exit(0);
	}
}


int myimport(int fd,char *file_name) {
	
	char *name_start=NULL;//Gia na bro to \n tis fgets
	char *delete;//Gia na bgalo to \n tis fgets kai na balw \0
	char name[MAX_STR_SIZE],name_with_path[MAX_STR_SIZE],tempname[MAX_STR_SIZE],answer;//Sto name bazeis to onoma to arxeiou pou perneis afairwntas to path kai sto tempname mpainei ena ena ta onomata
	//ton arxeion tis basis gia sigrisi
	int fd_new_file ,possible_error,more,db_size,found;
	unsigned int file_size;
	
	printf("Enter file name you want to import\n");
	getchar();
	fgets(name_with_path,MAX_STR_SIZE,stdin);
	if ((delete=strchr(name_with_path, '\n'))) {
		*delete = '\0';
	}
	name_start = strrchr(name_with_path,'/');//Se periptosi pou mou exei dosei path psaxno to teleutaio / gia na bro pou ksekinaei to onoma to arxeiou
	if (name_start ==NULL) {
		strcpy(name,name_with_path);//An den bro / den mou edose path
	}
	else {
		strcpy(name,name_start+1);//Allios ksekinao kai antigrafo apo tin epomeni desi
	}
	fd_new_file = open(name_with_path, O_RDONLY);
	if(fd_new_file == -1) {
		printf("%d: %s\n", __LINE__, strerror(errno));
	}
	else {
		found=1;//Den einai
		more=1;
		db_size = lseek(fd,0,SEEK_END);
		myerror(db_size,__LINE__);
		possible_error=lseek(fd,MAGIC_NUMBER_SIZE,SEEK_SET);
		myerror(possible_error,__LINE__);
		while(more>0){
			possible_error=read(fd,tempname,MAX_STR_SIZE);
			myerror(possible_error,__LINE__);
			found=strcmp(tempname,name);//An kanei 0 simainei oti brike arxeio me to idio onoma
			possible_error=read(fd,&file_size,sizeof(unsigned int));//Se kade periptosi diabazeis kai to megedos tou arxeiou
			myerror(possible_error,__LINE__);
			if (found==0) {
				break;
			}//Kai meta AN brikes arxeio me to idio onoma bganeis
			possible_error=lseek(fd,file_size,SEEK_CUR);//Allios proxoras sto epomeno arxeio
			myerror(possible_error,__LINE__);
			more=db_size-possible_error;//Sinexizeis tin epanalipsi mexri na ftaseis sto telos tis basis
		}
		if (found==0) {//An bgikes kai ontos to exeis brei to onoma pou psaxneis
			printf("File already exists.Are you sure you want to replace it?(y/n)");
			answer = getchar();
			getchar();
			if (answer == 'y' || answer == 'Y') {
				mydelete(fd,file_name,name);//Prota diagrafeis to palio	
			}
			else{
				return(0);
			}
		}//Kai epeita kanonika bazeis sto telos tis basis auto pou des
		possible_error=lseek(fd,0,SEEK_END);//Paei sto telos tis basis
		myerror(possible_error,__LINE__);
		possible_error=write(fd,&name[0],MAX_STR_SIZE);//Grafei to onoma pou exoume brei
		myerror(possible_error,__LINE__);
		file_size = lseek(fd_new_file,0,SEEK_END);//Paei sto telos tou arxeio gia na brei to megedos ton periexomenon
		myerror(file_size,__LINE__);
		possible_error=lseek(fd_new_file,0,SEEK_SET);//Girnaei stin arxi
		myerror(possible_error,__LINE__);
		possible_error = write(fd,&file_size,sizeof(unsigned int));//Grafei to megedos pou brike dipla apo to onoma
		myerror(possible_error,__LINE__);
		my_read_write(fd_new_file,file_size,fd);//Antigrafei ta periexomena
	}
	close(fd_new_file);
	return(0);
}


void my_read_write(int fd_source, int file_size, int fd_destination) {
	
	char buffer[BUFFERSIZE];
	int possible_error,fores,upoloipo,i;
	
	setvbuf(stdout , buffer, _IOFBF, BUFFERSIZE);
	fores = file_size/BUFFERSIZE;//Briskei posa 512 xorane
	upoloipo= file_size%BUFFERSIZE;//Posa bytes menoun
	
	for(i=0;i<fores;i++) {
		possible_error = read(fd_source,&buffer[0],BUFFERSIZE);
		myerror(possible_error,__LINE__);
		possible_error = write(fd_destination,&buffer[0],possible_error);
		myerror(possible_error,__LINE__);
	}
	possible_error=read(fd_source,&buffer[0],upoloipo);
	myerror(possible_error,__LINE__);
	possible_error = write(fd_destination,&buffer[0],possible_error);
	myerror(possible_error,__LINE__);
	setvbuf(stdout , buffer, _IOLBF, BUFFERSIZE);
}


int mysearch(int fd) {
	
	char name[MAX_STR_SIZE],tempname[MAX_STR_SIZE];//Sto name bazo to onoma tou arxeio pou anazito eno stos tempname bazo ena ena ta onomata ton arxeion pou iparxoun stis basi gia sigrisi
	char *delete,*found=NULL;
	int file_size,db_size,possible_error,more;
	int flag=0; //Den
	
	printf ("Enter Search Parameter\n");
	getchar();
	fgets (name,MAX_STR_SIZE,stdin);
	delete = strchr(name,'\n');//Pali brisko kai bgazo \n tis fgets
	*delete = '\0';
	db_size = lseek(fd,0,SEEK_END);//Brisko to megedos tis basis
	myerror(db_size,__LINE__);
	possible_error=lseek(fd,MAGIC_NUMBER_SIZE,SEEK_SET);//Pidao to magic number
	myerror(possible_error,__LINE__);
	if(db_size == MAGIC_NUMBER_SIZE){//Elenxo mipos i basi den exei tpt mesa kai stamatao tin anazitisi
		printf("Empty database\n");
		return(0);
	}
	more=1 ;//Arxikopoio gia na mpei ti proti fora stin epanalipsi
	if (strcmp(name,"*")!=0) {
		while(more>0){//Oso iparxoun akoma bytes sti basi
			possible_error=read(fd,tempname,MAX_STR_SIZE);//Diabazo onoma arxeiou apo ti basi
			myerror(possible_error,__LINE__);	
			found=strstr(tempname,name);//Psaxno an auto pou edose o xristis iparxei mesa se kapio name 
			if(found!=NULL){//An to bro emfanizo to name sto opio iparxei
				flag = 1;//Nai
				printf("%s\n",tempname);
			}
			possible_error=read(fd,&file_size,sizeof(unsigned int));//Diabazo to megedos tou arxeiou
			myerror(possible_error,__LINE__);
			possible_error=lseek(fd,file_size,SEEK_CUR);//To prospernao
			myerror(possible_error,__LINE__);
			more=db_size-possible_error;//Den ksexno na afereso to magedos tou arxeiou apo to megedos tis basis gia na do an exei noima na proxoriso
		}
		if (flag==0) {//An meta apo ola auta den bro tpt
		printf("Given substring does not exist in any file name\n");
		}
	}	
	else {
		printf("Showing All File Names\n");
		while(more>0){
			possible_error=read(fd,tempname,MAX_STR_SIZE);//Diabazo onoma
			myerror(possible_error,__LINE__);	
			printf("%s\n",tempname);//To ektipono
			flag=1;
			possible_error=read(fd,&file_size,sizeof(unsigned int));//Diabazo megedos
			myerror(possible_error,__LINE__);
			possible_error=lseek(fd,file_size,SEEK_CUR);//To prospernao
			myerror(possible_error,__LINE__);
			more=db_size-possible_error;//Den ksexno na afereso to magedos tou arxeiou apo to megedos tis basis gia na do an exei noima na proxoriso
		}
	}
	return(0);
}


void myerror(int error,int line){

	if (error < 0) {
		printf("%d : %s\n", line, strerror(errno));
		exit(0);
		
	}
}


int myexport(int fd) {
	
	char name_source[MAX_STR_SIZE],name_destination[MAX_STR_SIZE],tempname[MAX_STR_SIZE],answer;//Sto tempname grafo ena ena ta onomata ton arxeion tis basis gia na ta sigrino me auto pou edose o xristis
	char *delete;//Gia na bgalo ta \n kai apo ta 2 onomata pou perno apo fgets
	int more,found,possible_error,file_size,db_size,fd_new_file;
	
	printf("Enter file name to be exported\n");
	getchar();
	fgets(name_source,MAX_STR_SIZE,stdin);
	printf("Enter destination file name\n");
	fgets(name_destination,MAX_STR_SIZE,stdin);
	delete = strchr(name_source,'\n');
	*delete = '\0';
	delete = strchr(name_destination,'\n');
	*delete = '\0';
	db_size = lseek(fd,0,SEEK_END);//Brisko megedos basis
	myerror(db_size,__LINE__);
	possible_error=lseek(fd,MAGIC_NUMBER_SIZE,SEEK_SET);//Pidao to magic number
	myerror(possible_error,__LINE__);
	more = 1;//Gia na mpei stin while
	found = 1;//Den brika akoma arxeio me to idio onoma
	if(db_size == MAGIC_NUMBER_SIZE){
		printf("Empty database\n");//koitao gia keni basi kai stamatao an einai ontos keni
		return(0);
	}
	while(more>0){
		possible_error=read(fd,tempname,MAX_STR_SIZE);//Diabazo onoma apo ti basi
		myerror(possible_error,__LINE__);
		found=strcmp(tempname,name_source);//To elenxo me to onoma pou mou edose o xristis
		possible_error=read(fd,&file_size,sizeof(unsigned int));//Diabazo to megedos tou arxeiou
		myerror(possible_error,__LINE__);
		if (found==0) {
			break;
		}//An to bro bgaino
		possible_error=lseek(fd,file_size,SEEK_CUR);//Allios prospernao to arxeio auto
		myerror(possible_error,__LINE__);
		more=db_size-possible_error;//Den ksexnao na afereso to megedos apo ti basi gia na do an exei noima na proxorao
	}
	if (found!=0) {//Se periptosi pou meta tin anazitisi den bro arxeio me idio onoma
		printf("No such file exists in database\n");
	}
	else {//An bro to arxeio pou psaxnei o xristis
		fd_new_file = open(name_destination,O_RDWR|O_CREAT|O_EXCL,S_IRWXU);//Koitao an iparxei to arxeio sto opio zitise na ginei export
		if (fd_new_file == -1 && errno == EEXIST) {//An iparxei ton rotao ti delei na kanei
			printf("Do you want to replace the file? (y/n)\n");
			getchar();
			answer = getchar();
			if (answer == 'y' || answer == 'Y') {//Ama delei antikatastasi
				fd_new_file = open(name_destination,O_RDWR|O_TRUNC,S_IRWXU);
				myerror(fd_new_file,__LINE__);
				my_read_write(fd,file_size,fd_new_file);//Antigrafo ta periexomena
			}
		}
		else if (fd_new_file > 0) {
			my_read_write(fd,file_size,fd_new_file);//An to arxeio dimiourgidike antigrafo kanonika ta dedomena
		}
		else {
			printf("Unexpected Error\n");
		}
	close(fd_new_file);
	}
	return(0);
}



int mydelete(int fd,char *from,char *what){
	
	char *delete,tempname[MAX_STR_SIZE];//Sto tempname bazo ena ena ta onoma ton arxeion tis basis gia na ta sigrino me auto pou edose o xristis gia diagrafi
	int more,found,db_size,file_size,possible_error,fd1;
	
	
	delete = strchr(what,'\n');
	if(delete!=NULL){//Bgazo apo to name tou arxeiou pou delo na diagrapso to \n tis fgets
		*delete = '\0';
	}
	db_size = lseek(fd,0,SEEK_END);//Brisko megedos basis
	myerror(db_size,__LINE__);
	possible_error=lseek(fd,MAGIC_NUMBER_SIZE,SEEK_SET);//prospernao to magic number stin basi
	myerror(possible_error,__LINE__);
	more = 1;//Gia na mpei stin while
	found = 1;//Den to brika
	fd1=open(from,O_RDWR,S_IRWXU);//Anoigo kai deutero file descriptor gia tin basi
	myerror(fd1,__LINE__);
	possible_error=lseek(fd1,MAGIC_NUMBER_SIZE,SEEK_SET);//prospernao to magic number kai me ton deutero file descriptor
	myerror(possible_error,__LINE__);
	if(db_size == MAGIC_NUMBER_SIZE){
		printf("Empty database\n");//Elenxo gia keni basi
		return(0);
	}
	while(more>0){
		possible_error=read(fd,tempname,MAX_STR_SIZE);//Bazo onoma arxeiou apo ti basi sto tempname
		myerror(possible_error,__LINE__);
		found=strcmp(tempname,what);//Elenxo an einai auto pou psaxno
		possible_error=read(fd,&file_size,sizeof(unsigned int));//Den ksexno na diabaso kai to megedos tou arxeiou gia na mporo na proxoriso stin basi
		myerror(possible_error,__LINE__);
		possible_error=lseek(fd1,MAX_STR_SIZE+sizeof(unsigned int),SEEK_CUR);//Me ton deutero file descriptor prospernao to proto arxeio(onoma kai megedos tou)
		myerror(possible_error,__LINE__);
		possible_error=lseek(fd1,file_size,SEEK_CUR);//Proxorao oso to megedos tou arxeiou ston deutero file descriptor
		myerror(possible_error,__LINE__);
		if (found==0) {
			break;
		}// An to exo brei tote stamatao
		possible_error=lseek(fd,file_size,SEEK_CUR);//Proxorao kai ston arxiko file descriptor oso to megedos tou arxeiou
		myerror(possible_error,__LINE__);
		more=db_size-possible_error;//Den ksexno na bgalo megedos apo ti basi gia na do an iparxei logos na sinexiso
	}
	if (found!=0) {//An akoma den exo brei to arxeio pou psaxno
		printf("No such file exists in database\n");
	}
	else {//An to exo brei
		possible_error = lseek(fd,(-sizeof(unsigned int)-MAX_STR_SIZE),SEEK_CUR);//Pigaino pros ta piso osa bytes einai to onoma alla kai to megedos tou arxeiou
		myerror(possible_error,__LINE__);
		my_read_write(fd1,db_size-MAX_STR_SIZE-possible_error-file_size-sizeof(unsigned int),fd);//Antigrafo ekei ta dedomena pou ebgala
		ftruncate(fd,(db_size-file_size-sizeof(unsigned int)-MAX_STR_SIZE));//Den ksexnao na mirkino to arxeio
	}
	close(fd1);
	return(0);
}