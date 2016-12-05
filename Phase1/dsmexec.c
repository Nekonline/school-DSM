#include "common_impl.h"

/* variables globales */

/* un tableau gerant les infos d'identification */
/* des processus dsm */
dsm_proc_t *proc_array = NULL;

/* le nombre de processus effectivement crees */
volatile int num_procs_create = 0;

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
     pid_t pid;
     int num_procs = 0;
     int i,j,k;
     char ip[100];
     memset(&ip, '\0',100);
     /* ------ EXTRACT ARGV DATA-------*/
     char file_name[FILE_NAME_SIZE];
     strcpy(file_name,argv[1]);
     char proc_name[PROC_NAME_SIZE];
     strcpy(proc_name,argv[2]);

     /* buffer pour les pipes/sockets */
     char buffer[BUFFER_SIZE];

     /* -------socket---------*/
     struct sockaddr_in serv_addr;
     int master_sockfd;
     socklen_t len = sizeof(serv_addr);

      /* -------poll---------*/
     int timeout_msecs = -1;
     int ret;

     /* ----- DNS ------*/ //TODO
     char str_port[15];
     char hostname[128];

     /*   */
     char dsmwrap_path[1024]; //TODO

     /*************************************************************/
     /*Mise en place d'un traitant pour recuperer les fils zombies*/ //TODO
     /*************************************************************/
     /* XXX.sa_handler = sigchld_handler; */


     /*************************************************************/
     /*             lecture du fichier de machines                */
     /*************************************************************/
     /* 1- on recupere le nombre de processus a lancer */
     num_procs=count_line(file_name);
     /* 2- on recupere les noms des machines et leur rang dans un tableau de strucure  */
     dsm_proc_t machine_tab[num_procs]; //TODO
     init_machine_tab(file_name, machine_tab, num_procs);
     print_machine_tab(machine_tab, num_procs);



     /*************************************************************/
     /*              création des tableaux de pipe                */
     /*************************************************************/
     pipe_tab_t pipe_err_tab[num_procs];
     pipe_tab_t pipe_stdout_tab[num_procs];

     /*************************************************************/
     /*             création de poll                */
     /*************************************************************/
     struct pollfd fds[num_procs*2];


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
    } else{

        gethostname(hostname, sizeof hostname);
        //convertit le port en chaine de caracteres;
        sprintf(str_port,"%i",ntohs(serv_addr.sin_port));
        // ici on a pas encore récupéré l'adresse ip donc affichage -> 0
        hostname_to_ip(hostname , ip);
        printf("Port number %s et adresse ip %s : %s\n ", str_port, hostname, ip);
    }


     /*************************************************************/
     /*                   creation des fils                       */
     /*************************************************************/
     for(i = 0; i < num_procs ; i++) {

         /* creation du tube pour rediriger stdout */ //TODO faire dees if sur les pipes
         pipe(pipe_stdout_tab[i].pipefd);
         /* creation du tube pour rediriger stderr */
         pipe(pipe_err_tab[i].pipefd);

    	 pid = fork();
    	 if(pid == -1) ERROR_EXIT("fork");

         /*********************************/
         /*           fils                */
         /*********************************/
    	 if (pid == 0) {
            sleep(i);
            printf("\nPROCESSUS [%i]\n",getpid() );
            close(pipe_err_tab[i].pipefd[0]);
            close(pipe_stdout_tab[i].pipefd[0]);

            for (j=0; j<i ;j++){
                close(pipe_err_tab[j].pipefd[0]);
                close(pipe_stdout_tab[j].pipefd[0]);
            }
      	  //   /* redirection stdout */
            // dup2(pipe_stdout_tab[i].pipefd[1],STDOUT_FILENO); //  stdout becomes the synonymous with pipe_stdout[1] ;
      	  //   /* redirection stderr */
            // dup2(pipe_err_tab[i].pipefd[1],STDERR_FILENO);

            /***********************************************/
      	    /* Creation du tableau d'arguments pour le ssh */ // TODO faire une fonction
            /***********************************************/

           // argv = {dsmexec machine_file truc arg1 arg2 .... argn}
           // newargv = { "ssh","hostname distant", "dsmwrap", "port", "ip local", "cmd", "arg1", ..., "argn", NULL}
           char *newargv[argc+4];

           /* First arg is the cmd ssh */
           newargv[0] = "ssh";
           /* Second arg is the hostname of the distant machine*/
           newargv[1] = machine_tab[i].connect_info.name;
           /* Third arg is dsmwrap */
           getcwd(dsmwrap_path,1024);
           strcat(dsmwrap_path,"/bin/dsmwrap");
           newargv[2] = dsmwrap_path;
           /* 4th arg is the port */
           newargv[3] = str_port;
           /* 5th arg is the hostname*/
           newargv[4] = hostname;
           /* Last arg is NULL */
           newargv[argc+3] = NULL;

           for(k=0; k < argc-2; k++) {
              newargv[k+5] =argv[k+2];
          }
          /*
          for (k=0;k<= argc+3; k++) {
              if(newargv[k]== NULL)
               printf("arg [%i] -> NULL\n", k);
              else
               printf("arg [%i] = %s\n",k,newargv[k]);
           } */

           if ( execvp(newargv[0],newargv) ) {
                printf("execv failed with error %d %s\n",errno,strerror(errno));
                return 0;
            }


        /*********************************/
        /*            Pere               */
        /*********************************/
      	} else  if(pid > 0) {
      	   /* fermeture des extremites des tubes non utiles */
           close(pipe_err_tab[i].pipefd[1]);
           close(pipe_stdout_tab[i].pipefd[1]);

           /* initialisation de poll */
           fds[i].fd = pipe_stdout_tab[i].pipefd[0];
           fds[i+num_procs].fd = pipe_err_tab[i].pipefd[0];
           fds[i].events = POLLIN;
           fds[i+num_procs].events = POLLIN;

      	   num_procs_create++;
        }
     }


     /*********************************/
     /*           Network             */
     /*********************************/

     for(i = 0; i < num_procs ; i++){

        /* on accepte les connexions des processus dsm */
        printf(" TRY Accept : ok \n");

        printf("master_sockfd : %i\n",master_sockfd );
        printf("sin_addr : %i\n",serv_addr.sin_addr.s_addr );
        printf("port : %i\n",serv_addr.sin_port);



        machine_tab[i].connect_info.sockfd = do_accept(master_sockfd, (struct sockaddr*)&serv_addr, &len); //TODO verfifier ernno
        printf("Accept : ok \n");


        /*  On recupere le nom de la machine distante */
        /* 1- d'abord la taille de la chaine */
        /* 2- puis la chaine elle-meme */

        /* On recupere le pid du processus distant  */
        memset(&buffer, '\0', BUFFER_SIZE);
        //do_recv(machine_tab[i].connect_info.sockfd, buffer,BUFFER_SIZE);
        //machine_tab[i].pid = atoi(buffer);
        printf("PID : %i \n",machine_tab[i].pid );

        /* On recupere le numero de port de la socket */
        /* d'ecoute des processus distants */
        }

        /* envoi du nombre de processus aux processus dsm*/

        /* envoi des rangs aux processus dsm */

        /* envoi des infos de connexion aux processus */

        /* gestion des E/S : on recupere les caracteres */
        /* sur les tubes de redirection de stdout/stderr */
     while(1) {
         ret = poll(fds, num_procs_create, timeout_msecs);
         if (ret > 0) {
              /* An event on one of the fds has occurred for stdout. */
              for (i=0; i< num_procs_create; i++) {
                  if (fds[i].revents & POLLIN) {
                      memset(&buffer,'\0',BUFFER_SIZE);
                      read(pipe_stdout_tab[i].pipefd[0],buffer,BUFFER_SIZE);
                      fprintf(stdout, "[proc %i : %s: stdout] %s\n", i, machine_tab[i].connect_info.name,buffer);
                 }
             }
         }


         ret = poll(fds+num_procs_create, num_procs_create, timeout_msecs);
         if (ret > 0) {
            /* An event on one of the fds has occurred for stderr. */
            for (i=num_procs_create; i< 2*num_procs_create; i++) {
                if (fds[i].revents & POLLIN) {
                    memset(&buffer,'\0',BUFFER_SIZE);
                    read(pipe_err_tab[i-num_procs].pipefd[0],buffer,BUFFER_SIZE);
                    fprintf(stdout, "[proc %i : %s: stderr] %s\n", i-num_procs, machine_tab[i - num_procs].connect_info.name, buffer);
                }
            }
        }

    }


     /* on attend les processus fils */
     for (i=0; i<num_procs_create; i++){
         wait(NULL);
     }
     printf("-------> Wait des fils : ok\n");
     /* on ferme les descripteurs proprement */
     /*for (i=0; i<num_procs_create; i++){
         close(pipe_err_tab[i].pipefd[0]);
         close(pipe_stdout_tab[i].pipefd[0]);
         printf("Pere : fermeture de %i et %i\n",pipe_err_tab[j].pipefd[0],pipe_stdout_tab[j].pipefd[0]);
     }*/

     /* on ferme la socket d'ecoute */
  }
   exit(EXIT_SUCCESS);
}

/*ret = poll(fds, num_procs_create*, timeout_msecs);
if (ret > 0) {
     // An event on one of the fds has occurred.
     for (i=0; i< 2*num_procs_create; i++) {
         if (fds[i].revents & POLLIN) {

             memset(&buffer,'\0',BUFFER_SIZE);



             //read(pipe_stdout_tab[i].pipefd[0],buffer,BUFFER_SIZE);
             if (i<num_procs_create){
                 read(pipe_stdout_tab[i].pipefd[0],buffer,BUFFER_SIZE);
                 fprintf(stdout, "[proc %i : %s: stdout] %s\n", i, machine_tab[i].name,buffer);
            } else {
                read(pipe_err_tab[i-num_procs].pipefd[0],buffer,BUFFER_SIZE);
                fprintf(stdout, "[proc %i : %s: stderr] %s\n", i-num_procs, machine_tab[i - num_procs].name, buffer);

            }
        }
    }

}/*/
