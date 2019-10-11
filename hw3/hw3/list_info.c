/*
	**************************
	*Karamellios Sotiris 2237*
	* Vassilis Samaras 2144  *
	**************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structure_info.h"

void remove_proc_from_queue(process_T *process){
    int i=0;

    process->next->previous = process->previous;
    process->previous->next = process->next;

    while (process->arguments[i] != NULL) {
        free(process-> arguments[i]);
        i++;
    }
    free(process);
}

void print_delimiter() {
    printf("================================================================================\n");
}

void print_all_processes(process_T *list_head) {
    process_T *current;
    char *argum_print;
    int i;

    for ( current = list_head->next; current->pid != SENTINEL_PID; current = current->next) {
        print_delimiter();
        printf(">> pid: %d, process args: ",current->pid);
        if (strchr(current->name_of_prog,'/') != NULL) {
            printf("(%s " , current->name_of_prog);
        }
        else{
            printf("(./%s ",current->name_of_prog);
        }
        for (argum_print = current->arguments[1],i=1; argum_print!=NULL ; i++,argum_print = current->arguments[i]) {
            printf(", %s",argum_print);
        }
        printf(")");

        if (current->status == ACTIVE_PROC) {
            printf(" *(R)* ");
        }
        printf("\n");
    }
    print_delimiter();
}

void list_initialization(process_T **list_head){

    *list_head = (process_T *) malloc(sizeof(process_T));
    if (list_head == NULL) {
        fprintf(stderr, "Memory allocation has failed.Terminating..\n");
        exit(EXIT_FAILURE);
    }
    else{
        (*list_head)->next = *list_head;
        (*list_head)->previous = *list_head;
        (*list_head)->pid = SENTINEL_PID;
    }
}

process_T *scan_active_proc(process_T *list_head){
    process_T *current;

        for ( current = list_head->next; current->pid != SENTINEL_PID; current = current->next) {
            if (current->status == ACTIVE_PROC) {
                return(current);
            }
        }

    printf("No Active Process.\n");
    return(NULL);
}

void mark_next_active(process_T *process) {

    process->status = NEXT_PROC;
    if (process->previous->pid == SENTINEL_PID) {
        if (process->previous->previous->status != DEAD_MAN_WALKING) {
            process->previous->previous->status = UNACTIVE_PROC;
        }
    }
    else{
       process->previous->status = UNACTIVE_PROC;
    }

}

process_T *next_active_proc(process_T *list_head){
    process_T *current;

    current = scan_active_proc(list_head);
    if (current == NULL) {
        return(NULL);
    }
    else{
        if (current->next->pid == SENTINEL_PID) {
            current = current->next->next;
            return(current);
        }
        else{
            current = current->next;
            return(current);
        }
    }
}

process_T *scan_processes_pid(process_T *list_head,pid_t process_pid_to_be_removed){
    process_T *current;

        for ( current = list_head->next; current->pid != SENTINEL_PID; current = current->next) {
            if (current->pid == process_pid_to_be_removed) {
                return(current);
            }
        }

    return(NULL);
}

void add_process(process_T *list_head,process_T process_data) {
    process_T *new_node;

    new_node = (process_T *) malloc( sizeof(process_T) );
    if (new_node == NULL) {
        printf("Error allocating Memory.\n");
        exit(EXIT_FAILURE);
    }

    *new_node = process_data;

    new_node->next = head;
    new_node->previous = head->previous;
    new_node->next->previous = new_node;
    new_node->previous->next = new_node;
}

int is_empty(process_T *head){
    if (head->next == head && head->previous == head) {
        return(NO_PROC);
    }
    else{
        return(HAS_PROC);
    }
}

void skip_whitespaces(char *letter) {

    do {
        *letter = getchar();
    } while( *(letter) == ' ');
}


void add_terminating_char(char **argument,int argument_number,int term_char) {

    argument[argument_number] = realloc(argument[argument_number], term_char + 1);
    argument[argument_number][term_char] = '\0';
}

int get_args(char ***argv){
    int argc=0,i;
    char letter,**new_f_argv;

    new_f_argv = malloc ( sizeof (char *) );
    if (new_f_argv == NULL) {
        return(EXIT_FAILURE);
    }

    skip_whitespaces(&letter);
    if (letter == '\n') {
        return(-1);
    }

    while (1) {
        new_f_argv[argc] = (char *) malloc( ARG_MAXIM_LEN * sizeof(char) );
        if (new_f_argv[argc] == NULL) {
            printf("ERROR ALLOCATING MEMORY. Terminating...\n");
            exit(EXIT_FAILURE);
        }

        for (i = 0; letter != ' ' && letter != '\n'; i++) {
            new_f_argv[argc][i] = letter;
            letter = getchar();
        }

        add_terminating_char(new_f_argv,argc,i);
        argc++;
        new_f_argv = realloc(new_f_argv, sizeof(char*) * (argc + 1) );
        if (letter == '\n') {
            new_f_argv[argc] = NULL;
            break;
        }
        skip_whitespaces(&letter);
    }

    *argv = new_f_argv;
    return(argc);
}
