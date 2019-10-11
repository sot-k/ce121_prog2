/*
	**************************
	*Karamellios Sotiris 2237*
	* Vassilis Samaras 2144  *
	**************************
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

#define DESIRED_ARGS_NUM 5
#define BLOCK_SIGNAL 1

/*GLOBAL VARIABLES*/
volatile sig_atomic_t i=0;

void argc_check(int argc){
	if( argc != DESIRED_ARGS_NUM ){
	  fprintf (stderr,"Not the right number of arguments. Terminating...\n");
	  exit(EXIT_FAILURE);
	}
}

void argv_check(char const *argv[]){
	if( strcmp(argv[1],"-m") == 0){
		if( strcmp(argv[3],"-b") == 0){
			if( strcmp(argv[4],"0") == 0 || strcmp(argv[4],"1") == 0 ){
				return;
			}
			else{
				fprintf(stderr, "The fourth argument should be 1 if we wish to ignore the signal or 0 if we don\'t.\n");
				exit(EXIT_FAILURE);
			}
		}
		else{
			fprintf(stderr, "The third argument is invalid.\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		fprintf(stderr, "The first argument is invalid.\n");
		exit(EXIT_FAILURE);
	}
}

static void handler (int sig){
	i=0;
}

void block_check(int block_flag,int placement,sigset_t *set){
	struct sigaction act = { {0} };
	int returned_value;


	if( placement == 1){
		sigemptyset(set);
		sigaddset(set,SIGUSR1);
	}

	if(block_flag == BLOCK_SIGNAL){
		returned_value = sigprocmask(SIG_BLOCK,set,NULL);
		if (returned_value != 0) { //sunartisi error_check?
			fprintf(stderr, "Sigaction failed. Gotham has fallen.\n");
			perror("Sigaction ");
			fprintf(stderr, "Activating Knightfall protocol.\n");
			exit(EXIT_FAILURE);
		}
		else{
			return;
		}
	}
	else{
		act.sa_handler = handler ;
		returned_value = sigaction(SIGUSR1,&act,NULL);
		if (returned_value != 0) { //sunartisi error_check?
			fprintf(stderr, "Sigaction failed. Gotham has fallen.\n");
			perror("Sigaction ");
			fprintf(stderr, "Activating Knightfall protocol.\n");
			exit(EXIT_FAILURE);
		}
		returned_value=sigprocmask(SIG_UNBLOCK,set,NULL);
		if (returned_value != 0) { //sunartisi error_check?
			fprintf(stderr, "Sigaction failed. Gotham has fallen.\n");
			perror("Sigaction ");
			fprintf(stderr, "Activating Knightfall protocol.\n");
			exit(EXIT_FAILURE);
		}
	}
}


void check_counter (int counter){
	if(counter <= 0){
		fprintf(stderr, "Second argument should be a possitive number.\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char const *argv[]) {
	int counter, sig_restore = 1,placement=1;
	pid_t pid;
	sigset_t *set;

	set = (sigset_t *) malloc(sizeof(sigset_t));
	if (set==NULL) {
		printf("Failed.\n");
	}

    argc_check(argc);
	argv_check(argv);
	block_check(atoi(argv[4]),placement,set);
	placement++;

	counter = atoi(argv[2]);
	check_counter(counter);

	pid = getpid();

	while (i <= counter){
		fprintf(stdout, "pid :%d\t%d/%d\n", pid, i, counter);
		i++ ;
		sleep(5);
		if( (i == (counter/2) ) && (sig_restore == 1) ){
			block_check(0,placement,set);
			sig_restore = 0 ;
		}
	}
	
	free(set);
    return (0);
}
