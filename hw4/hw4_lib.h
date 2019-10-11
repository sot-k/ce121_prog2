/*
	**************************
	*Karamellios Sotiris 2237*
	* Vassilis Samaras 2144  *
	**************************
*/

#ifndef hw4_lib_h
#define hw4_lib_h

#include "debug.h"
#include <sys/types.h>

#define MESSAGE_LEN (64+1)
#define MEM_KEY 1337
#define NAME_LEN (32+1)
#define COUNTER_SIZE (( sizeof(int) )+1)
#define MEM_SIZE ((2 * MESSAGE_LEN) + (2 * NAME_LEN) + COUNTER_SIZE )
// +1 gia ta \0 pou xreiazontai

struct message{
    long type;
    char minima[100];
};typedef struct message message_my_t;
#define READ1_READY 999
#define READ2_READY 666
#define WRITE1_READY 333
#define	WRITE2_READY 111

void set_memo(char *shmid,const char first_name[],const char second_name[]);
void sync_initialization();
int check_names(char *shmid,const char first_name[],const char second_name[]);
void availability(int flag);
void completed(int flag);


#endif
