#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>


/* autres includes (eventuellement) */

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

struct pipe_tab  {
   int pipefd[2];
};
typedef struct pipe_tab pipe_tab_t;

// Gestion fichiers
int count_line(char * filename);
void init_machine_tab(char * filemane, dsm_proc_t *machine_tab, int nb_machine);
void print_machine_tab(dsm_proc_t *machine_tab, int nb_machine);

// Gestion IP
void init_serv_address(struct sockaddr_in *serv_addr_ptr);
void init_client_address(struct sockaddr_in *serv_addr_ptr, char *port,char *adresse_ip);
int create_socket();
void do_bind(int sockfd, struct sockaddr_in *serv_addr_ptr);
void do_send(int sockfd, char *buffer, int buffer_size);
int do_recv(int sockfd, char *buffer, int buffer_size);
int do_accept(int sockfd, struct sockaddr *address, socklen_t *address_len);
int do_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int hostname_to_ip(char * hostname , char* ip);
