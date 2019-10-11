/*
	**************************
	*Karamellios Sotiris 2237*
	* Vassilis Samaras 2144  *
	**************************
*/

#ifndef structure_info_h
#define structure_info_h

#define MAX_PROG_NAME 256

struct process_info{
    pid_t pid; //process id
    char name_of_prog[MAX_PROG_NAME];
    char **arguments;
    int status;
    struct process_info *next;
    struct process_info *previous;
}typedef process_T;

process_T *head;

void print_delimiter();
void remove_proc_from_queue(process_T *process);
void print_all_processes(process_T *list_head);
void list_initialization(process_T **list_head);
process_T *scan_active_proc(process_T *list_head);
void mark_next_active(process_T *process);
process_T *next_active_proc(process_T *list_head);
process_T *scan_processes_pid(process_T *list_head,pid_t process_pid_to_be_removed);
void add_process(process_T *list_head,process_T process_data);
int is_empty(process_T *head);
void add_terminating_char(char **argument,int argument_number,int term_char);
void skip_whitespaces(char *letter);
int get_args(char ***argv);

#define SENTINEL_PID 666
#define NO_PROC 911
#define HAS_PROC 448
#define ACTIVE_PROC 278
#define NEXT_PROC 2016
#define UNACTIVE_PROC 777
#define DELETE_PROC 456
#define ARG_MAXIM_LEN (4096 + 1)
#define EXEC_F 12
#define TERM_F 14
#define SIG_F 78
#define INFO_F 84
#define QUIT_F 555
#define DEAD_MAN_WALKING 999

#endif
