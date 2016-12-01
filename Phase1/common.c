#include "common_impl.h"

int creer_socket(int prop, int *port_num)
{
   int fd = 0;

   /* fonction de creation et d'attachement */
   /* d'une nouvelle socket */
   /* renvoie le numero de descripteur */
   /* et modifie le parametre port_num */

   return fd;
}

/* Vous pouvez ecrire ici toutes les fonctions */
/* qui pourraient etre utilisees par le lanceur */
/* et le processus intermediaire. N'oubliez pas */
/* de declarer le prototype de ces nouvelles */
/* fonctions dans common_impl.h */

int count_line(char * filename) {
  FILE *fd = NULL;
  int compteur = 0;
  int buffer_size = 100;
  char str[buffer_size];

  fd=fopen(filename,"r");
  if(fd == NULL) {
      ERROR_EXIT("Error opening file")
   }
   while( fgets (str, buffer_size, fd)!=NULL ) {
      compteur++;
   }
   fclose(fd);

   return(compteur);
}

void init_machine_tab( char * filename, dsm_proc_conn_t *machine_tab, int nb_machine) {
  FILE *fd = NULL;
  int i, j;

  fd=fopen(filename,"r");
  if(fd == NULL) {
      ERROR_EXIT("Error opening file")
   }
   for (i=0 ; i<nb_machine ; i++) {
     machine_tab[i].rank = i;
     fgets(machine_tab[i].name, NAME_SIZE, fd);
     // Replace the \n at the end of the string with \0
     for (j=0 ; j<NAME_SIZE ; j++){
       if ( machine_tab[i].name[j] == '\n' )
        machine_tab[i].name[j] = '\0';
     }
   }
   fclose(fd);
}

void print_machine_tab(dsm_proc_conn_t *machine_tab, int nb_machine) {
  int i;
  for (i=0 ; i<nb_machine ; i++) {
     printf("Machine [%i]\'s name is %s \n", machine_tab[i].rank, machine_tab[i].name);
   }
}


void init_serv_address(struct sockaddr_in *serv_addr_ptr) {
  memset(serv_addr_ptr, 0, sizeof(struct sockaddr_in));
  serv_addr_ptr->sin_family = AF_INET;
  serv_addr_ptr->sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY : all interfaces - not just "localhost", multiple network interfaces OK
  serv_addr_ptr->sin_port = 0;              /* bind() will choose a random port*/
}

int create_socket() {
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);   //Sockets config: Blocking  //Possible to add SO_REUSEADDR with setsockopt() during dev phase testing...etc
  if (sockfd < 0) {
    ERROR_EXIT("Error - socket opening");
  }

  return sockfd;
}

void do_bind(int sockfd, struct sockaddr_in *serv_addr_ptr) {
  if ( bind(sockfd, (struct sockaddr *) serv_addr_ptr, sizeof(struct sockaddr_in))<0 ) {  //cast generic struct
    ERROR_EXIT("Error - bind");
  }
}

void do_send(int sockfd, char *buffer, int buffer_size) {
  int progress = 0; //total sent
  int sent = 0; //each try
  do {
    if ( (sent = send(sockfd, buffer+sent, buffer_size-sent, 0)) < 0 ) {
      ERROR_EXIT("Error - send");
    }
    progress += sent;
  } while(progress != buffer_size);
}

int do_recv(int sockfd, char *buffer, int buffer_size) {
  int progress = 0; //total sent
  int read = 0; //each try
  do {
    if ( (read = recv(sockfd, buffer+read, buffer_size-read, 0)) < 0 ) {
      ERROR_EXIT("Error - recv");
    }
    else if(read == 0) {  //Connection closed abruptly by remote peer, receiving 0 bytes will have the same effect
      close(sockfd);
      return -1; //CLOSE_ABRUPT;
    }
    progress += read;
  } while(progress != buffer_size);

  return progress;
}


int do_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen){

	int res=connect(sockfd,addr,addrlen);
	while (res!=0){
		res=connect(sockfd,addr,addrlen);
	}
}


int do_accept(int sockfd, struct sockaddr *address, socklen_t *address_len){
    int new_sock=accept(socket,address,address_len);
    return new_sock;
}
