#include "common_impl.h"

int main(int argc, char **argv)
{
    printf("******************************\n");
    printf("    ON EST DANS LE DSMWRAP     \n");
    printf("******************************\n");

    fflush(stdout);


    /*********************************************************/
    /*          Initialisation des variables                 */
    /*********************************************************/
    int k=0;
    char *newargv[argc-3];
    /* -------socket---------*/
    int sockfd_procs, master_sockfd;
    struct sockaddr_in procs_addr, serv_addr;
    int len =sizeof(serv_addr);
    char buffer[BUFFER_SIZE];
    /* ----- DNS ------*/ //TODO
    char str_port[15];
    char hostname[128];
    char ip[100];


    hostname_to_ip(argv[2] , ip);
    init_client_address(&procs_addr, argv[1],ip);
    printf("%s resolved to %s, port %i: \n" , argv[2] , ip, procs_addr.sin_port );
    fflush(stdout);

   /*********************************************************/
   /*             Connexion avec master                     */
   /*********************************************************/
   /* creation d'une socket */
   sockfd_procs = create_socket();
   /* connexion */


   printf("master_sockfd : %i\n",sockfd_procs );
   printf("sin_addr : %i\n",procs_addr.sin_addr.s_addr );
   printf("port : %i\n",procs_addr.sin_port);
   fflush(stdout);


   do_connect(sockfd_procs, (const struct sockaddr*)&procs_addr, sizeof(procs_addr)); //TODO synchro
   printf(" Connecté au master\n");

   /* Envoi du nom de machine au lanceur */

   /* Envoi du pid au lanceur */ //TODO
   memset(&buffer, '\0',BUFFER_SIZE );
   sprintf(buffer,"%d",getpid());
   do_send(master_sockfd, buffer, BUFFER_SIZE);
   printf(" PID envoyé\n");




   /*************************************/
   /* Creation d'un serveur dans le wrap */
   /*************************************/

   /* Creation de la socket d'ecoute pour les */
   /* connexions avec les autres processus dsm */

  //  init_serv_address(&serv_addr);
  //  /* creation de la socket d'ecoute */
  //  master_sockfd = create_socket();
  //  /* bind - choix du port non determiné */
  //  do_bind(master_sockfd, &serv_addr);
  //  /* + ecoute effective */
  //  if(listen(master_sockfd, 100) < 0) //TODO
  //      ERROR_EXIT("Error - listen");
  //
  //  if (getsockname(master_sockfd, (struct sockaddr *)&serv_addr, &len) == -1){
  //     ERROR_EXIT("getsockname");
  // } else{
  //
  //     gethostname(hostname, sizeof hostname);
  //     //convertit le port en chaine de caracteres;
  //     sprintf(str_port,"%d",ntohs(serv_addr.sin_port));
  //     // ici on a pas encore récupéré l'adresse ip donc affichage -> 0
  //     printf("Port number %s et adresse ip %s\n ", str_port,hostname);
  // }


   /* Envoi du numero de port au lanceur */
   /* pour qu'il le propage à tous les autres */
   /* processus dsm */

   /*********************************************************/
   /*             Création tableau exec                     */
   /*********************************************************/

  /* First arg is the cmd */
  newargv[0] = argv[3];
  /* Last arg is NULL */
  newargv[argc-3] = NULL;

  for(k=0; k < argc-3; k++) {
     newargv[k+1] =argv[k+4];
 }

 for (k=0;k<= argc-3; k++) {
     if(newargv[k]== NULL)
      printf("arg [%i] -> NULL\n", k);
     else
      printf("arg [%i] = %s\n",k,newargv[k]);
  }

   /* on execute la bonne commande */
   return 0;
}
