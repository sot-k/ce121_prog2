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
#include <signal.h>
#include "structure_info.h"

#define READ_LEN 256

int get_choice(){
    char input[READ_LEN+1];
    int check_val;

    check_val = scanf(" %256s",input);
    if (check_val < 0) {
        return(check_val);
    }

    if (strcmp(input,"exec") == 0) {
        return(EXEC_F);
    }
    else if (strcmp(input,"info") == 0) {
        return(INFO_F);
    }
    else if (strcmp(input,"term") == 0) {
        return(TERM_F);
    }
    else if (strcmp(input,"sig") == 0) {
        return(SIG_F);
    }
    else if (strcmp(input,"quit") == 0) {
        return(QUIT_F);
    }

    return(-1);
}

void print_available_commands(){
    print_delimiter();
    printf("Dear,user\n");
    printf("Because we don't want you to get us in trouble.. we inform you that\n");
    printf("we offer as commands:\n");
    printf(" >exec\n >term\n >sig\n >info\n >quit\n");
    printf("Thank you for using our software. (:\n");
    print_delimiter();
}

void unblock_sigusr1() {
    int returned_value;
    struct sigaction act = { {0} };

    act.sa_handler = SIG_IGN;
    returned_value = sigaction(SIGUSR1,&act,NULL);
    if (returned_value != 0) { //sunartisi error_check?
        fprintf(stderr, "Sigaction failed. Gotham has fallen.\n");
        perror("Sigaction ");
        fprintf(stderr, "Activating Knightfall protocol.\n");
        exit(EXIT_FAILURE);
    }
}

void my_exec() {
    char **args;
    process_T process_to_add;
    pid_t process_id;
    int checker;

    get_args(&args);


    process_id = fork();
    if (process_id == -1) {
        perror("fork");
    }
    else{
        if (process_id == 0) {
            unblock_sigusr1();
            execv(args[0],args);
            perror("execv failed");
            exit(EXIT_FAILURE);
        }
        process_to_add.pid = process_id;
        strcpy(process_to_add.name_of_prog,args[0]);
        process_to_add.arguments = args;
        checker = is_empty(head);
        if (checker == NO_PROC) {
            process_to_add.status = ACTIVE_PROC;
        }
        else{
            if ( scan_active_proc(head) == NULL) {
                process_to_add.status = ACTIVE_PROC;
            }
            else{
                process_to_add.status = UNACTIVE_PROC;
                kill(process_to_add.pid,SIGSTOP);
            }
        }
        add_process(head,process_to_add);

    }
}

static void handler_schild (int sig){
    int status,pid_changed,signal_caused;
    process_T *process,*process_next;

    pid_changed = waitpid(-1,&status,WNOHANG);
    if (WIFEXITED(status)) {
        process = scan_processes_pid(head,pid_changed);
        if (process != NULL) {
            if (process->status == ACTIVE_PROC) {
                process_next = next_active_proc(head);
                if (process_next!= NULL) {
                    kill(process_next->pid,SIGCONT);
                    process_next->status = ACTIVE_PROC;
                }
            }
            process->status = DEAD_MAN_WALKING;
        }
    }
    else{
        if (WIFSIGNALED(status)) {
            signal_caused = WTERMSIG(status);
            if (signal_caused == SIGSTOP || signal_caused == SIGCONT) {
                // no actions needed. Default.
            }
            else{
                if (signal_caused == SIGTERM || signal_caused == SIGKILL) {
                    process = scan_processes_pid(head,pid_changed);
                    if (process != NULL) {
                        if (process->status == ACTIVE_PROC) {
                            process_next = next_active_proc(head);
                            if (process_next!= NULL) {
                                kill(process_next->pid,SIGCONT);
                                process_next->status = ACTIVE_PROC;
                            }
                        }
                        process->status = DEAD_MAN_WALKING;
                    }
                }
            }
        }
    }
}

void remove_dead_proc_via_pid() {
    process_T *current,*to_be_removed;

    for ( current = head->next; current->pid != SENTINEL_PID; current = current->next){
        if (current->status == DEAD_MAN_WALKING) {
            to_be_removed = current;
            current = to_be_removed->previous;
            remove_proc_from_queue(to_be_removed);
        }
    }
}

void my_sigterm() {
    pid_t pid_to_sterm;

    scanf("\n%d",&pid_to_sterm);
    kill(pid_to_sterm,SIGTERM);
    printf(">>>>Process with pid %d has been slaughtered.<<<<\n",pid_to_sterm);
}

void my_quit() {
    process_T *current;

    for ( current = head->next; current->pid != SENTINEL_PID; current = current->next){
        kill(current->pid,SIGKILL);//LALIS SAID KILL DESE NOOBS WITH SIG_KILL
    }

    remove_dead_proc_via_pid();
    free(head);

    printf(">>>>Processes had been slaughtered.<<<<\n");
    printf(">>>>Everybody is dead now..........<<<<\n");
    printf(">>>>             RIP               <<<<\n");
}

void my_sigusr1() {
    pid_t pid_to_sigusr1;

    scanf("\n%d",&pid_to_sigusr1);
    kill(pid_to_sigusr1,SIGUSR1);
    printf(">>>>Process with pid %d has been SIGUSR1ed.<<<<\n",pid_to_sigusr1);
}

static void handler_sigALRM(int sig) {
    int check;
    process_T *process_next,*process_running;

    write(STDOUT_FILENO,"got SIGALRM\n",12);

    check = is_empty(head);
    if (check == NO_PROC) {
        write(STDOUT_FILENO,"NO PROC RUNNING\n",17);
        return;
    }
    else{
        process_running = scan_active_proc(head);
        process_next = next_active_proc(head);
        if (process_next!= NULL) {
            mark_next_active(process_next);
            kill(process_running->pid,SIGSTOP);
            kill(process_next->pid,SIGCONT);
            process_next->status = ACTIVE_PROC;
        }
    }
}

int main(int argc, char const *argv[]) {
    int choice,returned_value,checker;
    struct sigaction actalrm = { {0} };
    struct itimerval timer = { {0} };
    struct sigaction act = { {0} };

    act.sa_handler = SIG_IGN;
    returned_value = sigaction(SIGUSR1,&act,NULL);
    if (returned_value != 0) {
        fprintf(stderr, "Sigaction failed. Gotham has fallen.\n");
        perror("Sigaction ");
        fprintf(stderr, "Activating Knightfall protocol.\n");
        exit(EXIT_FAILURE);
    }


    act.sa_handler = handler_schild ;
    sigaction(SIGCHLD,&act,NULL);

    actalrm.sa_handler = handler_sigALRM;
    actalrm.sa_flags=SA_RESTART;
    sigaction(SIGALRM,&actalrm,NULL);

    timer.it_interval.tv_sec = 20;
    timer.it_interval.tv_usec = 0;
    timer.it_value = timer.it_interval;

    checker = setitimer(ITIMER_REAL,&timer,NULL);
    if ( checker == -1 ) {
        perror("setitimer");
        return(-5);
    }

    print_available_commands();
    list_initialization(&head);

    while (1) {
        choice = get_choice();
        remove_dead_proc_via_pid();

        switch(choice){
          case EXEC_F: {
              my_exec();
              break;
	        }
	      case TERM_F: {
             my_sigterm();
	         break;
	      }
	      case SIG_F: {
	         my_sigusr1();
	         break;
	      }
	      case INFO_F: {
             checker = is_empty(head);
             if (checker == HAS_PROC) {
                 print_all_processes(head);
             }
             else{
                 printf("*Zero processes running.*\n");
             }
	         break;
	      }
	      case QUIT_F: {
	         my_quit();
	         return(0);
         }
         default :{
             break;
         }
	   }
    }

    return (0);
}
