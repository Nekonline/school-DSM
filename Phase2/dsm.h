#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>




/* fin des includes */

#define TOP_ADDR    (0x40000000)
#define PAGE_NUMBER (100)
#define PAGE_SIZE   (sysconf(_SC_PAGE_SIZE))
#define BASE_ADDR   (TOP_ADDR - (PAGE_NUMBER * PAGE_SIZE))
#define ERROR_EXIT(str) {perror(str);exit(EXIT_FAILURE);}
#define NAME_SIZE 30
#define FILE_NAME_SIZE 30
#define PROC_NAME_SIZE 30
#define ARG_NAME_SIZE 30
#define PIPE_SIZE 300
#define BUFFER_SIZE 1024


/* definition du type des infos */
/* de connexion des processus dsm */
struct dsm_proc_conn  {
    int sockfd;
    int rank;
    int port;
    char name[NAME_SIZE];
};
typedef struct dsm_proc_conn dsm_proc_conn_t;

/* definition du type des infos */
/* d'identification des processus dsm */
struct dsm_proc {
  pid_t pid;
  dsm_proc_conn_t connect_info;
};
typedef struct dsm_proc dsm_proc_t;


typedef enum
{
   NO_ACCESS,
   READ_ACCESS,
   WRITE_ACCESS,
   UNKNOWN_ACCESS
} dsm_page_access_t;

typedef enum
{
   INVALID,
   READ_ONLY,
   WRITE,
   NO_CHANGE
} dsm_page_state_t;

typedef int dsm_page_owner_t;

typedef struct
{
   dsm_page_state_t status;
   dsm_page_owner_t owner;
} dsm_page_info_t;

dsm_page_info_t table_page[PAGE_NUMBER];

pthread_t comm_daemon;
extern int DSM_NODE_ID;
extern int DSM_NODE_NUM;

char *dsm_init( int argc, char **argv);
void  dsm_finalize( void );
