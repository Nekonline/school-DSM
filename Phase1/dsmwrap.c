#include "common_impl.h"

int main(int argc, char **argv)
{
    /*********************************************************/
    /* Initialisation des variables pour la connexion reseau */
    /*********************************************************/
    int sockfd_procs, sockfd_master;
    struct sockaddr_in procs_addr;

    //TODO implementer le reseau
   /* processus intermediaire pour "nettoyer" */
   /* la liste des arguments qu'on va passer */
   /* a la commande a executer vraiment */

   /* creation d'une socket pour se connecter au */
  /* au lanceur et envoyer/recevoir les infos */
  /* necessaires pour la phase dsm_init */

    //sockfd_master = create_socket();
    //sockfd_procs = create_socket();


    /*************************************/
    /* Creation d'un serveur ans le wrap */
    /*************************************/

    //init_serv_address(procs_addr);



   /* Envoi du nom de machine au lanceur */

   /* Envoi du pid au lanceur */

   /* Creation de la socket d'ecoute pour les */
   /* connexions avec les autres processus dsm */

   /* Envoi du numero de port au lanceur */
   /* pour qu'il le propage à tous les autres */
   /* processus dsm */

   /* on execute la bonne commande */
   return 0;
}
