#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

/* autres includes (eventuellement) */

#define ERROR_EXIT(str) {perror(str);exit(EXIT_FAILURE);}
#define NAME_SIZE 30
#define FILE_NAME_SIZE 30
#define PROC_NAME_SIZE 30
#define ARG_NAME_SIZE 30


/* definition du type des infos */
/* de connexion des processus dsm */
struct dsm_proc_conn  {
   int rank;
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

int creer_socket(int type, int *port_num);
int count_line(char * filename);
void init_machine_tab(char * filemane, dsm_proc_conn_t *machine_tab, int nb_machine);
void print_machine_tab(dsm_proc_conn_t *machine_tab, int nb_machine);
