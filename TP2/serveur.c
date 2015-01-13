/* T. Grandpierre - Application distribue'e pour TP IF4-DIST 2004-2005

But : 

fournir un squelette d'application capable de recevoir des messages en 
mode non bloquant provenant de sites connus. L'objectif est de fournir
une base pour implementer les horloges logique/vectorielle/scalaire, ou
bien pour implementer l'algorithme d'exclusion mutuelle distribue'

Syntaxe :
				 arg 1 : Numero du 1er port
	 arg 2 et suivant : nom de chaque machine

--------------------------------
Exemple pour 3 site :

Dans 3 shells lances sur 3 machines executer la meme application:

pc5201a>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr
pc5201b>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr
pc5201c>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr

pc5201a commence par attendre que les autres applications (sur autres
sites) soient lance's

Chaque autre site (pc5201b, pc5201c) attend que le 1er site de la
liste (pc5201a) envoi un message indiquant que tous les sites sont lance's


Chaque Site passe ensuite en attente de connexion non bloquante (connect)
sur son port d'ecoute (respectivement 5000, 5001, 5002).
On fournit ensuite un exemple permettant 
1) d'accepter la connexion 
2) lire le message envoye' sur cette socket
3) il est alors possible de renvoyer un message a l'envoyeur ou autre si
necessaire 

*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include "queue.c"

#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

char **Sites;
int *Ports;

int horloge=0;
int id;
struct queue *FILEi;

int *Responses;

int NSites;

int isInSC = 0;



int GetSitePos(int Nbsites, char *argv[]) ;
void WaitSync(int socket);
void SendSync(char *site, int Port);
void getSites(int NbSites, char *argv[]);

/*Identification de ma position dans la liste */
int GetSitePos(int NbSites, char *argv[]) {
	char MySiteName[20]; 
	int MySitePos=-1;
	int i;
	gethostname(MySiteName, 20);
	for (i=0;i<NbSites;i++) 
		if (strcmp(MySiteName,argv[i+2])==0) {
			MySitePos=i;
			//printf("L'indice de %s est %d\n",MySiteName,MySitePos);
			return MySitePos;
		}
	if (MySitePos == -1) {
		printf("Indice du Site courant non trouve' dans la liste\n");
		exit(-1);
	}
	return (-1);
}

void getSites(char *argv[]){
	char MySiteName[20]; 
	int i;
	int num=0;
	int port0 = atoi(argv[1]);
	gethostname(MySiteName, 20);
	for (i=0;i<NSites;i++) {
		strcpy(Sites[num], argv[i+2]);
		if(strcmp(Sites[i],MySiteName)) id = i;
		Ports[num] = port0;
		num++;
		port0++;
	}
}

int sendRequest(char* request){
	int i;
	struct hostent* hp;
	int sock;
	socklen_t size;
	struct sockaddr_in sock_add;
	char MySiteName[20];
	gethostname(MySiteName, 20);
	for(i=0;i<NSites;i++){
		if(!strcmp(Sites[i], MySiteName)){
			hp = gethostbyname(Sites[i]);
			if(hp==NULL) return -1;
			size = sizeof(sock_add);

			sock_add.sin_family = AF_INET;
			sock_add.sin_port = htons(Ports[i]);
			memcpy(&sock_add.sin_addr.s_addr, hp->h_addr, hp->h_length);
			if ( (sock=socket(AF_INET, SOCK_STREAM,0))==-1) {
				perror("Creation socket");
				return -1;
			}
			if (connect(sock, (struct sockaddr*) &sock_add,size )==-1) {
				perror("Probleme connect");
				return -1;
			}
			write(sock,request,strlen(request)+1);
			close(sock);
		}
	}
	return 0;
}

/* Permet de construire une char* sous la forme horloge|id|msg */
void codeMessage(const char* msg, char* msg_out){
	sprintf(msg_out, "%d|%d|%s", horloge, id, msg);
}

void decodeMessage(const char *message_in, int *horloge, int *machine, char *message_out){
	sscanf(message_in,"%d|%d|%[^\n]",horloge, machine, message_out);
}




/*Attente bloquante d'un msg de synchro sur la socket donne'e*/
void WaitSync(int s_ecoute) {
	char texte[40];
	int l;
	int s_service;
	struct sockaddr_in sock_add_dist;
	socklen_t size_sock;
	size_sock=sizeof(struct sockaddr_in);
	printf("WaitSync : ");fflush(0);
	s_service=accept(s_ecoute,(struct sockaddr*) &sock_add_dist,&size_sock);
	l=read(s_service,texte,39);
	texte[l] ='\0';
	printf("%s\n",texte); fflush(0);
	close (s_service);
} 

/*Envoie d'un msg de synchro a la machine Site/Port*/
void SendSync(char *Site, int Port) {
	struct hostent* hp;
	int s_emis;
	char chaine[15];
	int longtxt;
	struct sockaddr_in sock_add_emis;
	int size_sock;
	
	if ( (s_emis=socket(AF_INET, SOCK_STREAM,0))==-1) {
		perror("SendSync : Creation socket");
		exit(-1);
	}
		
	hp = gethostbyname(Site);
	if (hp == NULL) {
		perror("SendSync: Gethostbyname");
		exit(-1);
	}

	size_sock=sizeof(struct sockaddr_in);
	sock_add_emis.sin_family = AF_INET;
	sock_add_emis.sin_port = htons(Port);
	memcpy(&sock_add_emis.sin_addr.s_addr, hp->h_addr, hp->h_length);
	
	if (connect(s_emis, (struct sockaddr*) &sock_add_emis,size_sock)==-1) {
		perror("SendSync : Connect");
		exit(-1);
	}
		 
	sprintf(chaine,"**SYNCHRO**");
	longtxt =strlen(chaine);
	/*Emission d'un message de synchro*/
	write(s_emis,chaine,longtxt);
	close (s_emis); 
}

void sendRelease(){
	char *msg = (char*) malloc(50*sizeof(char));
	sprintf(msg, "%d|%d|release", horloge, id);
	sendRequest(msg);
}

void sendRequest(){
	push(FILEi,id);
	char *msg = (char*) malloc(50*sizeof(char));
	sprintf(msg, "%d|%d|request", horloge, id);
	sendRequest(msg);
}

void sendResponse(){
	char *msg = (char*) malloc(50*sizeof(char));
	sprintf(msg, "%d|%d|response", horloge, id);
	sendRequest(msg);
}

void receiveRelease(int horloge, int id){
	
}

void receiveRequest(int horloge, int id){
	
}

void receiveResponse(int horloge, int id){
	
}

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

int main (int argc, char* argv[]) {

	FILEi = (struct queue*) malloc(sizeof(struct queue));


	struct sockaddr_in sock_add, sock_add_dist;
	socklen_t size_sock;
	int s_ecoute, s_service;
	char texte[40];
	int i,l;
	//float t;

	int PortBase=-1; /*Numero du port de la socket a` creer*/
	NSites=-1; /*Nb total de sites*/


	if (argc<3) {
		printf("Erreur: il faut donner au moins 2 sites pour faire fonctionner l'application: NumeroPortBase et liste_des_sites");
		exit(-1);
	}

	/*----Nombre de sites (adresses de machines)---- */
	NSites=argc-2;

	// Allocation des serveurs/ports
	Sites = (char**) malloc(NSites * sizeof(char*));
	Ports = (int*) malloc(NSites * sizeof(int));
	Responses = (int*) malloc(NSites * sizeof(int));

	for(i=0;i<NSites;i++){
		Sites[i] = (char*) malloc(20*sizeof(char));
	}

	getSites(NSites, argv);

	srand(time(NULL));

	/*CREATION&BINDING DE LA SOCKET DE CE SITE*/
	PortBase=atoi(argv[1])+GetSitePos(NSites, argv);
	printf("Numero de port de ce site %d\n",PortBase);

	sock_add.sin_family = AF_INET;
	sock_add.sin_addr.s_addr= htons(INADDR_ANY);  
	sock_add.sin_port = htons(PortBase);

	if ( (s_ecoute=socket(AF_INET, SOCK_STREAM,0))==-1) {
		perror("Creation socket");
		exit(-1);
	}

	if ( bind(s_ecoute,(struct sockaddr*) &sock_add, \
			sizeof(struct sockaddr_in))==-1) {
		perror("Bind socket");
		exit(-1);
	}
	
	listen(s_ecoute,30);
	/*----La socket est maintenant cre'e'e, binde'e et listen----*/

	if (GetSitePos(NSites, argv) ==0) { 
		/*Le site 0 attend une connexion de chaque site : */
		for(i=0;i<NSites-1;i++) 
			WaitSync(s_ecoute);
		printf("Toutes les synchros recues \n");fflush(0);
		/*et envoie un msg a chaque autre site pour les synchroniser */
		for(i=0;i<NSites-1;i++) 
			SendSync(argv[3+i], atoi(argv[1])+i+1);
		} else {
			/* Chaque autre site envoie un message au site0 
	 (1er  dans la liste) pour dire qu'il est lance'*/
			SendSync(argv[2], atoi(argv[1]));
			/*et attend un message du Site 0 envoye' quand tous seront lance's*/
			printf("Wait Synchro du Site 0\n");fflush(0);
			WaitSync(s_ecoute);
			printf("Synchro recue de  Site 0\n");fflush(0);
	}

	
	/* Passage en mode non bloquant du accept pour tous*/
	/*---------------------------------------*/
	fcntl(s_ecoute,F_SETFL,O_NONBLOCK);
	size_sock=sizeof(struct sockaddr_in);
	
	/* Boucle infini*/
	while(1) {
	
		/* On commence par tester l'arriv�e d'un message */
		s_service=accept(s_ecoute,(struct sockaddr*) &sock_add_dist,&size_sock);
		if (s_service>0) {
			/*Extraction et affichage du message */
			l=read(s_service,texte,20);
			texte[l] ='\0';
			fflush(0);
			int horloge_received, id_received;
			char* msg_received=NULL;
			decodeMessage(texte, &horloge_received, &id_received, msg_received);

			horloge = max(horloge, horloge_received);

			if(!strcmp(msg_received, "release")){
				receiveRelease(horloge_received,id_received);
			}else if(!strcmp(msg_received, "request")){
				receiveRequest(horloge_received,id_received);
			}else if(!strcmp(msg_received, "response")){
				receiveResponse(horloge_received,id_received);
			}

			// push in queue
			
			if(rand()%10 > 7 && !isInSC) {
				// demand SC
				isInSC=1;
				sendRequest(NSites);
			}

			close (s_service);
		}
	

		/* Petite boucle d'attente : c'est ici que l'on peut faire des choses*/
		usleep(100000);
		
		printf(".");fflush(0); /* pour montrer que le serveur est actif*/
	}


	close (s_ecoute);  
	return 0;
}

