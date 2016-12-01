#include "common_impl.h"

/* variables globales */

/* un tableau gerant les infos d'identification */
/* des processus dsm */
dsm_proc_t *proc_array = NULL;

/* le nombre de processus effectivement crees */
volatile int num_procs_creat = 0;

void usage(void)
{
  fprintf(stdout,"Usage : dsmexec machine_file executable arg1 arg2 ...\n");
  fflush(stdout);
  exit(EXIT_FAILURE);
}

void sigchld_handler(int sig)
{
   /* on traite les fils qui se terminent */
   /* pour eviter les zombies */
}


int main(int argc, char *argv[])
{
  if (argc < 3){
    usage();
  } else {
      /*************************************************************/
      /*              Initialisation des variables                 */
      /*************************************************************/
     pid_t pid;
     int num_procs = 0;
     int i;
     /* ------ EXTRACT ARGV DATA-------*/
     char file_name[FILE_NAME_SIZE];
     strcpy(file_name,argv[1]);
     char proc_name[PROC_NAME_SIZE];
     strcpy(proc_name,argv[2]);

     /* ------INIT PIPELINE-------*/
     int pipe_stdout[2], pipe_stderr[2];
     /* creation du tube pour rediriger stdout */
     pipe(pipe_stdout);
     /* creation du tube pour rediriger stderr */
     pipe(pipe_stderr);
     /* buffer pour les pipes */
     char buffer[PIPE_SIZE];

     /* -------socket---------*/
     struct sockaddr_in serv_addr;
     int master_sockfd;
     socklen_t len = sizeof(serv_addr);


     /*************************************************************/
     /*Mise en place d'un traitant pour recuperer les fils zombies*/ //TODO
     /*************************************************************/
     /* XXX.sa_handler = sigchld_handler; */


     /*************************************************************/
     /*             lecture du fichier de machines                */
     /*************************************************************/
     /* 1- on recupere le nombre de processus a lancer */
     num_procs=count_line(file_name);
     printf("Le fichier %s contient %i machines.\n", file_name, num_procs );
     /* 2- on recupere les noms des machines et leur rang dans un tableau de strucure  */
     dsm_proc_conn_t machine_tab[num_procs];
     init_machine_tab(file_name, machine_tab, num_procs);
     print_machine_tab(machine_tab, num_procs);


     /*************************************************************/
     /*            socket - création, bind, listen...             */
     /*************************************************************/

     init_serv_address(&serv_addr);
     /* creation de la socket d'ecoute */
     master_sockfd = create_socket();
     /* bind - choix du port non determiné */
     do_bind(master_sockfd, &serv_addr);
     /* + ecoute effective */
     if(listen(master_sockfd, num_procs) < 0)
         ERROR_EXIT("Error - listen");

     if (getsockname(master_sockfd, (struct sockaddr *)&serv_addr, &len) == -1){
        ERROR_EXIT("getsockname");
     } else
        printf("port number %d\n", ntohs(serv_addr.sin_port));


     /*************************************************************/
     /*                   creation des fils                       */
     /*************************************************************/
     for(i = 0; i < num_procs ; i++) {

    	 pid = fork();
    	 if(pid == -1) ERROR_EXIT("fork");

    	 if (pid == 0) { /* fils */
            close(pipe_stderr[0]);
            close(pipe_stdout[0]);
      	    /* redirection stdout */
            dup2(pipe_stdout[1],STDOUT_FILENO); //  stdout becomes the synonymous with pipe_stdout[1] ;
      	    /* redirection stderr */
            dup2(pipe_stderr[1],STDERR_FILENO);

           printf("Gestion des pipes : ok\n");
      	   /* Creation du tableau d'arguments pour le ssh */ // TODO faire une fonction
           char *newargv[argc-1];
           /* First arg is the proc name */
           newargv[0] = argv[2];
           /* Last arg is NULL */
           newargv[argc-1] = NULL;
            /* Store the arg in a table newargv */
           if (argc > 3){
               for(i=0; i < argc-3; i++) {
                  newargv[i+1] =argv[i+3];
              }
           }
           printf("Tableau argument: ok\n");

           if ( execvp(newargv[0],newargv) )
                printf("execv failed with error %d %s\n",errno,strerror(errno));

      	} else  if(pid > 0) { /* pere */
      	   /* fermeture des extremites des tubes non utiles */
           close(pipe_stderr[1]);
           close(pipe_stdout[1]);

           // TODO enlever

           char buffer[PIPE_SIZE];
           read(pipe_stdout[0],buffer,PIPE_SIZE);
           printf("%s\n",buffer );

      	   num_procs_creat++; // TODO wait
        }
     }

     printf("MY PID: %i \n", getpid());
     for(i = 0; i < num_procs ; i++){

	/* on accepte les connexions des processus dsm */

	/*  On recupere le nom de la machine distante */
	/* 1- d'abord la taille de la chaine */
	/* 2- puis la chaine elle-meme */

	/* On recupere le pid du processus distant  */

	/* On recupere le numero de port de la socket */
	/* d'ecoute des processus distants */
     }

     /* envoi du nombre de processus aux processus dsm*/

     /* envoi des rangs aux processus dsm */

     /* envoi des infos de connexion aux processus */

     /* gestion des E/S : on recupere les caracteres */
     /* sur les tubes de redirection de stdout/stderr */
     /* while(1)
         {
            je recupere les infos sur les tubes de redirection
            jusqu'à ce qu'ils soient inactifs (ie fermes par les
            processus dsm ecrivains de l'autre cote ...)

         };
      */

     /* on attend les processus fils */

     /* on ferme les descripteurs proprement */

     /* on ferme la socket d'ecoute */
  }
   exit(EXIT_SUCCESS);
}
