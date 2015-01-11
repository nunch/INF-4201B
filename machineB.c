#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	struct sockaddr_in ad_emeteur;
	struct sockaddr_in ad_recepteur;
	unsigned int taille = sizeof(ad_emeteur);
	int res;
	char tab[10];
	int s = socket(AF_INET,SOCK_DGRAM,0);
	ad_recepteur.sin_port = htons(9990);
	ad_recepteur.sin_addr.s_addr = INADDR_ANY;
	memset((char*) &ad_recepteur, 0, sizeof(ad_recepteur));
	res = bind(s,(struct sockaddr *)&ad_recepteur,sizeof(ad_recepteur));
	int n;
	n = recvfrom(s, tab,sizeof(tab), 0,(struct sockaddr *) &ad_emeteur, &taille);
	printf("machine = %s\n",tab);
	strcpy(tab,"ça marche");
	n = sendto(s, tab, sizeof(tab),0,(struct sockaddr *) &ad_emeteur,sizeof(ad_emeteur));
	return 0;
}