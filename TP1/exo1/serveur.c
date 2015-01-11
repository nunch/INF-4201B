#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	if(argc!=2){
		printf("usage\n");
		return 0;
	}
	int port_recepteur = atoi(argv[1]);
	struct sockaddr_in ad_recepteur,ad_emeteur;
	socklen_t taille = sizeof(ad_emeteur);
	int s,res;
	char message[50];
	char pid[20];
	s = socket(AF_INET,SOCK_DGRAM,0);
	if (s==-1)
	{
		perror("socket\n");
		return 0;
	}
	char hostname[50];
	gethostname(hostname,sizeof(hostname));
	printf("%s\n",hostname);
	
	ad_recepteur.sin_port = htons(port_recepteur);
	ad_recepteur.sin_addr.s_addr = INADDR_ANY;
	ad_recepteur.sin_family=AF_INET;
	memset((char*) &ad_recepteur.sin_zero, 0, 8);

	res = bind(s,(struct sockaddr *)&ad_recepteur,sizeof(ad_recepteur));
	if (res==-1)
	{
		perror("bind\n");
		return 0;
	}
	int n;
	n = recvfrom(s, pid,sizeof(pid), 0,(struct sockaddr *) &ad_emeteur, &taille);
	printf("pid = %s\n", pid);
	n = recvfrom(s, message,sizeof(message), 0,(struct sockaddr *) &ad_emeteur, &taille);
	printf("message = %s\n",message);
	sprintf(message,"%s %s",pid,message);
	printf("%s\n", message);
	sendto(s,message,sizeof(message),0,(struct sockaddr *)&ad_emeteur,sizeof(ad_emeteur));

	return 0;
}