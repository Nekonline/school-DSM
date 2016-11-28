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
