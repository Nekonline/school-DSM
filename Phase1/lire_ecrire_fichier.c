#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// 1 Ouvrir le fichier en mode R ou R +w

// 2 On compte le nombre de linges dans le fichier

// 3 On alloue un tableau de chaines
// Le tableau a pour taille le nombre de lignes
// Une chaine a une taille max SIZE_MAX 64

//4 On recupere dans le tableau les mots dans le fichier

// 1

int main(int argc,char **argv){
  FILE *fd=NULL;
  fd=fopen("jojo.txt","r");

  char caractere;
  int compteur=0;
  int i=1;


  /*while((caractere=fgetc(fd)) != EOF){
    if(caractere=='\n'){
      compteur+=1;
    }
  }*/

  // méthode statique
  //char tab[N][SIZE];

  char *tab[20];
  // méthode dynamique
  char *t=malloc(100);

  while (fgets(t, 100, fd) != NULL){


      //printf("la ligne numero%d dit %s\n",i,t);

      tab[i-1]=malloc(100);
      t[strlen(t)-1]='\0';
      strcpy(tab[i-1],t);
      printf("%s\n",tab[i-1]);
      i++;


}
printf("il y %i lignes\n",i-1);

}
