#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


/*
 * Un client HTTP en mode connecté avec gestion du proxy
 * 
 * @param
 * Le nom du serveur auquel on va chercher à se connecter,
 * Le numéro de port de ce serveur,
 * Un nom de fichier.
 */
int main(int argc, char const *argv[]) {
	if(argc != 4) {
		printf("3 paramètres attendu\n");
		printf("Le nom du serveur auquel on va chercher à se connecter,\n");
		printf("Le numéro de port de ce serveur,\n");
		printf("Un nom de fichier.\n");
		return -1;
	}

	// On récupère les paramètres données
	char host[50];
	strcpy(host,argv[1]);
	char fichier[50];
	strcpy(fichier,argv[3]);

	// On déclare les variables
	struct sockaddr_in ad;
	struct hostent *h = NULL;
	char str[1024];
	char request[1024];
	int l;

	// On démarre le socket en mode connecté
	int s = socket(AF_INET,SOCK_STREAM,0);
	if(s == -1)	{ perror("Error socket");	return -1; }

	h = gethostbyname("proxy.esiee.fr");
	if(h == NULL) { perror("Error gethostbyname"); return -1;	}

	ad.sin_port = htons(3128);
	ad.sin_family = h->h_addrtype;
	memcpy(&(ad.sin_addr.s_addr), h->h_addr, h->h_length);
	memset(&(ad.sin_zero), '\0', 8);

	// On connecte le client au serveur
	int res = connect(s,(struct sockaddr*)&ad,sizeof(ad));
	if(res == -1) { perror("Error connect"); return -1; }

	// On prépare puis on envoie la requête HTTP
	sprintf(request, "GET");
	sprintf(request, "%s http://www.%s:%s/%s", request, host, argv[2], fichier);
	sprintf(request, "%s HTTP/1.1\r\nHost: ", request);
	sprintf(request, "%s%s\r\n", request, host);
	sprintf(request, "%sConnection: close\r\n", request);
	sprintf(request, "%s\r\n", request);
	int w = write (s, request, strlen(request));
	if(w == -1) { perror("Error write"); return -1; }

	// On fait une boucle sur la lecture jusqu'à ce qu'on est reçu l'intégralité des données
    while ((l = read (s, str, sizeof (str) - 1))){
    	if(l == -1) { perror("Error read"); return -1; }
    	str[l] = 0;
    	printf ("%s", str);
    }

	// On ferme le socket
    shutdown (s, 2);
    close(s);
	printf("\n\nREQUETE\n%s\n", request);
	return 0;
}
