#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	if(argc!=4){
		printf("usage\n");
		return 0;
	}
	char host[50];
	strcpy(host,argv[1]);
	int port_server = atoi(argv[2]);
	char message[50];
	char pid[20];
	sprintf(pid,"moi");
	strcpy(message,argv[3]);
	struct hostent *h=NULL;
	int s,test,res;

	s = socket(AF_INET,SOCK_DGRAM,0);
	if (s==-1)
	{
		perror("socket");
		return 0;
	}

	struct sockaddr_in ad_dest;
	socklen_t taille = sizeof(ad_dest);
	ad_dest.sin_port = htons(port_server);
	ad_dest.sin_family = AF_INET;
	h = gethostbyname(host);
	if (h==NULL)
	{
		perror("gethostbyname");
		return 0;
	}
	memcpy(&(ad_dest.sin_addr.s_addr), h->h_addr, h->h_length);

	/*res = bind(s,(struct sockaddr *)&ad_dest,sizeof(ad_dest));
	if (res==-1)
	{
		perror("bind");
		return 0;
	}*/

	res = sendto(s, pid, sizeof(pid),0,(struct sockaddr *)&ad_dest,sizeof(ad_dest));
	res = sendto(s, message, sizeof(message),0,(struct sockaddr *)&ad_dest,sizeof(ad_dest));
	printf("%d\n", res);
	res = recvfrom(s, message,sizeof(message), 0,(struct sockaddr *) &ad_dest, &taille);
	printf("%s\n", message);
	return 0;
}