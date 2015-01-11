/* T. Grandpierre@esiee.fr TP IF4-DIST 2004-2005

But : petit client qui se connecte plusieurs fois sur le meme port
de la meme machine pour envoyer le meme message afin de tester le 
serveur

args :


-machine serveur
-port serveur
-message 
-nombre de copies du meme message (a chaque fois un connect)
*/


#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>



int main (int argc, char* argv[]) {
  struct sockaddr_in sock_add;
  int client;
  int size_sock_add;
  char chaine[40];
  int i,l,longtxt;
  struct hostent* hp;


  if (argc != 5 ) {
    printf("Erreur arguments : NomMachine NumeroPort Message NbEnvois\n");
    exit(-1);
  }

  // Recup adresse 
  hp = gethostbyname(argv[1]);
  if (hp == NULL) {
		perror("client");
		return -1;
  }

  size_sock_add=sizeof(struct sockaddr_in);
  sock_add.sin_family = AF_INET;
  sock_add.sin_port = htons(atoi(argv[2]));
  memcpy(&sock_add.sin_addr.s_addr, hp->h_addr, hp->h_length);



  for (i=0;i<atoi(argv[4]);i++) {
    if ( (client=socket(AF_INET, SOCK_STREAM,0))==-1) {
      perror("Creation socket");
      exit(-1);
    }
    
    if (connect(client, (struct sockaddr*) &sock_add,size_sock_add )==-1) {
      perror("Probleme connect"); }
    else {
      sprintf(chaine,"%s - %i",argv[3], i);
      longtxt =strlen(chaine);
      l=write(client,chaine,longtxt+1);
      close (client);
    }
  }
  exit(0);  
}