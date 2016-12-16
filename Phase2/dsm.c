#include "dsm.h"

int DSM_NODE_NUM; /* nombre de processus dsm */
int DSM_NODE_ID;  /* rang (= numero) du processus */

/* indique l'adresse de debut de la page de numero numpage */
static char *num2address( int numpage )
{
   char *pointer = (char *)(BASE_ADDR+(numpage*(PAGE_SIZE)));

   if( pointer >= (char *)TOP_ADDR ){
      fprintf(stderr,"[%i] Invalid address !\n", DSM_NODE_ID);
      return NULL;
   }
   else return pointer;
}

/* fonctions pouvant etre utiles */
static void dsm_change_info( int numpage, dsm_page_state_t state, dsm_page_owner_t owner)
{
   if ((numpage >= 0) && (numpage < PAGE_NUMBER)) {
	if (state != NO_CHANGE )
	table_page[numpage].status = state;
      if (owner >= 0 )
	table_page[numpage].owner = owner;
      return;
   }
   else {
	fprintf(stderr,"[%i] Invalid page number !\n", DSM_NODE_ID);
      return;
   }
}

static dsm_page_owner_t get_owner( int numpage)
{
   return table_page[numpage].owner;
}

static dsm_page_state_t get_status( int numpage)
{
   return table_page[numpage].status;
}

/* Allocation d'une nouvelle page */
static void dsm_alloc_page( int numpage )
{
   char *page_addr = num2address( numpage );
   mmap(page_addr, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
   return ;
}

/* Changement de la protection d'une page */
static void dsm_protect_page( int numpage , int prot)
{
   char *page_addr = num2address( numpage );
   mprotect(page_addr, PAGE_SIZE, prot);
   return;
}

static void dsm_free_page( int numpage )
{
   char *page_addr = num2address( numpage );
   munmap(page_addr, PAGE_SIZE);
   return;
}

static void *dsm_comm_daemon( void *arg)
{
   while(1)
     {
	/* a modifier */
	printf("[%i] Waiting for incoming reqs \n", DSM_NODE_ID);
	sleep(2);
     }
   return;
}

static int dsm_send(int dest,void *buf,size_t size)
{
    int progress = 0; //total sent
    int sent = 0; //each try
    do {
      if ( (sent = send(dest, buf+sent, size-sent, 0)) < 0 ) {
        ERROR_EXIT("Error  - send");
      }
      progress += sent;
    } while(progress != size);
    return progress;
}

static int dsm_recv(int from,void *buf,size_t size)
{
    int progress = 0; //total sent
    int read = 0; //each try
    do {
      if ( (read = recv(from, buf+read, size-read, 0)) < 0 ) {
        ERROR_EXIT("Error - recv");
      }
      else if(read == 0) {  //Connection closed abruptly by remote peer, receiving 0 bytes will have the same effect
        close(from);
        return -1; //CLOSE_ABRUPT;
      }
      progress += read;
    } while(progress != size);

    return progress;
}

static void dsm_handler( void )
{
   /* A modifier */
   printf("[%i] FAULTY  ACCESS !!! \n",DSM_NODE_ID);
   abort();
}

/* traitant de signal adequat */
static void segv_handler(int sig, siginfo_t *info, void *context)
{
   /* A completer */
   /* adresse qui a provoque une erreur */
   void  *addr = info->si_addr;
  /* Si ceci ne fonctionne pas, utiliser a la place :*/
  /*
   #ifdef __x86_64__
   void *addr = (void *)(context->uc_mcontext.gregs[REG_CR2]);
   #elif __i386__
   void *addr = (void *)(context->uc_mcontext.cr2);
   #else
   void  addr = info->si_addr;
   #endif
   */
   /*
   pour plus tard (question ++):
   dsm_access_t access  = (((ucontext_t *)context)->uc_mcontext.gregs[REG_ERR] & 2) ? WRITE_ACCESS : READ_ACCESS;
  */
   /* adresse de la page dont fait partie l'adresse qui a provoque la faute */
   void  *page_addr  = (void *)(((unsigned long) addr) & ~(PAGE_SIZE-1));

   if ((addr >= (void *)BASE_ADDR) && (addr < (void *)TOP_ADDR))
     {
	dsm_handler();
     }
   else
     {
	/* SIGSEGV normal : ne rien faire*/
     }
}

static void print_machine_tab(dsm_proc_t *machine_tab, int nb_machine) {
  int i;
  for (i=0 ; i<nb_machine ; i++) {
     printf("Machine [%i]\'s  name is %s \n", machine_tab[i].connect_info.rank, machine_tab[i].connect_info.name);
    fflush(stdout);
   }
}

int do_accept(int sockfd, struct sockaddr *address, socklen_t *address_len){
    int new_sock = accept(sockfd,address,address_len) ;
    if( new_sock==-1) ERROR_EXIT("Error accept");
    return new_sock;
}

int do_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen){

	int res=connect(sockfd,addr,addrlen);
	while (res!=0){
		res=connect(sockfd,addr,addrlen);
        if (res==-1) {
            printf("waiting accept ...%s\n",strerror(errno));
            fflush(stdout);
            sleep(1);
        }
    }
    return res;
}


void init_serv_address(struct sockaddr_in *serv_addr_ptr) {
  memset(serv_addr_ptr, 0, sizeof(struct sockaddr_in));
  serv_addr_ptr->sin_family = AF_INET;
  serv_addr_ptr->sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY : all interfaces - not just "localhost", multiple network interfaces OK
  serv_addr_ptr->sin_port = 0;              /* bind() will choose a random port*/
}

void init_client_address(struct sockaddr_in * serv_addr_ptr, char *port,char *adresse_ip) {
    struct in_addr addr_client;


    memset(serv_addr_ptr, 0, sizeof(struct sockaddr_in));
    serv_addr_ptr->sin_family = AF_INET;
    //inet_aton(adresse_ip, serv_addr_ptr->sin_addr);
    inet_aton(adresse_ip,&addr_client);
    serv_addr_ptr->sin_addr.s_addr = addr_client.s_addr;  //INADDR_ANY : all interfaces - not just "localhost", multiple network interfaces OK
    serv_addr_ptr->sin_port = htons(atoi(port));
    //printf("init_cli -->  PORT client = %i = %s = %d\n", atoi(port), port, ntohs(serv_addr_ptr->sin_port) );
}

/* Seules ces deux dernieres fonctions sont visibles et utilisables */
/* dans les programmes utilisateurs de la DSM                       */
char *dsm_init(int argc, char **argv)
{

   /*********************************************************/
   /*          Initialisation des variables                 */
   /*********************************************************/
   struct sigaction act;
   int index;
   int maestro_sockfd = 3, master_sockfd = 4;
   char buffer[BUFFER_SIZE];
   int i,k;
   struct sockaddr_in addr_provs;

   /***********************************************************/
   /*              Echange de donnée de connexion             */
   /***********************************************************/

   /* reception du nombre de processus dsm envoye */
   /* par le lanceur de programmes (DSM_NODE_NUM)*/
   memset(&buffer, '\0',BUFFER_SIZE );
   dsm_recv(maestro_sockfd, buffer, BUFFER_SIZE);
   DSM_NODE_NUM = atoi(buffer);
   printf(" num_procs :  %i\n", DSM_NODE_NUM);
   fflush(stdout);

   /* reception de mon numero de processus dsm envoye */
   /* par le lanceur de programmes (DSM_NODE_ID)*/
   memset(&buffer, '\0',BUFFER_SIZE );
   dsm_recv(maestro_sockfd, buffer, BUFFER_SIZE);
   DSM_NODE_ID = atoi(buffer);
   printf(" rank :  %i\n",  DSM_NODE_ID);
   fflush(stdout);

   /* reception des informations de connexion des autres */
   /* processus envoyees par le lanceur : */
   /* nom de machine, numero de port, etc. */
   dsm_proc_t machine_tab[ DSM_NODE_NUM];
   memset(&buffer, '\0',BUFFER_SIZE );
   dsm_recv(maestro_sockfd, buffer, BUFFER_SIZE);
   memcpy(&machine_tab, &buffer , sizeof(machine_tab));
   printf(" Recuperation des infos de connexion aux processus : ok\n");
   fflush(stdout);
   print_machine_tab(machine_tab,  DSM_NODE_NUM);

   /* initialisation des connexions */
   /* avec les autres processus : connect/accept */

   for (k=0;k<DSM_NODE_ID;k++){
       do_connect()
   }



   /* Allocation des pages en tourniquet */
   for(index = 0; index < PAGE_NUMBER; index ++){
     if ((index % DSM_NODE_NUM) == DSM_NODE_ID)
       dsm_alloc_page(index);
     dsm_change_info( index, WRITE, index % DSM_NODE_NUM);
   }

   /* mise en place du traitant de SIGSEGV */
   act.sa_flags = SA_SIGINFO;
   act.sa_sigaction = segv_handler;
   sigaction(SIGSEGV, &act, NULL);

   /* creation du thread de communication */
   /* ce thread va attendre et traiter les requetes */
   /* des autres processus */
   pthread_create(&comm_daemon, NULL, dsm_comm_daemon, NULL);

   /* Adresse de début de la zone de mémoire partagée */
   return ((char *)BASE_ADDR);
}

void dsm_finalize( void )
{
   /* fermer proprement les connexions avec les autres processus */

   /* terminer correctement le thread de communication */
   /* pour le moment, on peut faire : */
   pthread_cancel(comm_daemon);

  return;
}
