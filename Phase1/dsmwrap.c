#include "common_impl.h"

int main(int argc, char **argv)
{
    printf("\n******************************\n");
    printf("    ON EST DANS LE DSMWRAP     \n");
    printf("******************************\n");

    fflush(stdout);


    /*********************************************************/
    /*          Initialisation des variables                 */
    /*********************************************************/
    int k=0;
    char *newargv[argc-3];
    /* -------socket---------*/
    int num_procs, rank;
    int maestro_sockfd, master_sockfd;
    struct sockaddr_in maestro_addr, serv_addr;
    int len =sizeof(serv_addr);
    char buffer[BUFFER_SIZE];
    /* ----- DNS ------*/ //TODO
    char master_port[15];

    char hostname[128];
    char ip[100];


    hostname_to_ip(argv[2] , ip);
    init_client_address(&maestro_addr, argv[1],ip);
    //printf("Client : %s resolved to %s, port %d = %s \n" , argv[2] , ip, ntohs(maestro_addr.sin_port),argv[1] );
    //fflush(stdout);

   /*********************************************************/
   /*             Connexion avec master                     */
   /*********************************************************/

   /* creation d'une socket */
   maestro_sockfd = create_socket();
   printf("maestro : %i", maestro_sockfd);
   fflush(stdout);


   /* connexion */
   do_connect(maestro_sockfd, (const struct sockaddr*)&maestro_addr, sizeof(maestro_addr)); //TODO synchro
   /* Envoi du nom de machine au lanceur */

   /* Envoi du pid au lanceur */ //TODO
   memset(&buffer, '\0',BUFFER_SIZE );
   sprintf(buffer,"%d",getpid());
   do_send(maestro_sockfd, buffer, BUFFER_SIZE);


   /***********************************************************/
   /*              Creation d'un serveur dans le wrap         */
   /***********************************************************/

   init_serv_address(&serv_addr);
   /* creation de la socket d'ecoute */
   master_sockfd = create_socket();
   printf("master socket : %i", master_sockfd);
   fflush(stdout);
   /* bind - choix du port non determiné */
   do_bind(master_sockfd, &serv_addr);
   /* + ecoute effective */
   if(listen(master_sockfd, 100) < 0) //TODO
       ERROR_EXIT("Error - listen");

   if (getsockname(master_sockfd, (struct sockaddr *)&serv_addr, &len) == -1) {
      ERROR_EXIT("getsockname");
  } else {
      gethostname(hostname, sizeof hostname);
      //convertit le port en chaine de caracteres;
      sprintf(master_port,"%d",ntohs(serv_addr.sin_port));
      // ici on a pas encore récupéré l'adresse ip donc affichage -> 0
      hostname_to_ip(hostname , ip);
      //printf("procs i : Port number %s et adresse ip %s : %s\n ", master_port, hostname, ip);
      //fflush(stdout);
  }


    /***********************************************************/
    /*              Echange de donnée de connexion             */
    /***********************************************************/

    /* Envoi du numero de port au lanceur pour qu'il le propage à tous les autres processus dsm */
    memset(&buffer, '\0',BUFFER_SIZE );
    sprintf(buffer,"%d",ntohs(serv_addr.sin_port));
    do_send(maestro_sockfd, buffer, BUFFER_SIZE);
    printf("Check point !\n");

    // /* Recuperation du nombre de processus */
    // memset(&buffer, '\0',BUFFER_SIZE );
    // do_recv(maestro_sockfd, buffer, BUFFER_SIZE);
    // num_procs = atoi(buffer);
    // printf(" num_procs :  %i\n", num_procs);
    // fflush(stdout);
    // /* Recuperation du rang */
    // memset(&buffer, '\0',BUFFER_SIZE );
    // do_recv(maestro_sockfd, buffer, BUFFER_SIZE);
    // rank = atoi(buffer);
    // printf(" rank :  %i\n", rank);
    // fflush(stdout);
    //
    // /* Recuperation des infos de connexion aux processus */
    // dsm_proc_t machine_tab[num_procs];
    // memset(&buffer, '\0',BUFFER_SIZE );
    // do_recv(maestro_sockfd, buffer, BUFFER_SIZE);
    // memcpy(&machine_tab, &buffer , sizeof(machine_tab));
    // printf(" Recuperation des infos de connexion aux processus : ok\n");
    // fflush(stdout);
    // print_machine_tab(&machine_tab, num_procs);


   /*********************************************************/
   /*             Création tableau exec                     */
   /*********************************************************/

    /* First arg is the cmd */
    newargv[0] = argv[4];
    /* Last arg is NULL */
    newargv[argc-3] = NULL;

    for(k=0; k < argc-3; k++) {
     newargv[k+1] =argv[k+4];
    }

    // for (k=0;k<= argc-3; k++) {
    //  if(newargv[k]== NULL)
    //   printf("arg [%i] -> NULL\n", k);
    //  else
    //   printf("arg [%i] = %s\n",k,newargv[k]);
    // }


    /*********************************************************/
    /*                         Exec                          */
    /*********************************************************/
    chdir(argv[3]);
    char file[30];
    sprintf(file,"%s",newargv[0]);

    printf(" Execvp wrap\n" );
    /* on execute la bonne commande */
    if ( execvp(file,newargv)){
        printf("execv failed with error %d %s\n",errno,strerror(errno));
        return 0;
    }
    return 0;
}
