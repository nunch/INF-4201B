
#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char const *argv[])
{
	int s,res;
	struct sockaddr_in ad1,ad2;
	s = socket(AF_INET,SOCK_DGRAM,0);
	if(s==-1) printf("fuck 1\n");;
	ad1.sin_family = AF_INET;
	ad1.sin_port = htons(9999);
	ad1.sin_addr.s_addr = INADDR_ANY;
	memset((char*) &ad1, 0, sizeof(ad1));
	res = bind(s,(struct sockaddr *)&ad1,sizeof(ad1));
	if(res == -1) printf("fuck 2\n");
	ad2.sin_port = htons(9990);
	struct hostent *h = gethostbyname("machineB");
	memcpy(&(ad2.sin_addr.s_addr), h->h_addr, h->h_length);
	char tab[10] = "Bonjour";
	res = sendto(s, tab, sizeof(tab),0,(struct sockaddr *)&ad2,sizeof(ad2));
	return 0;
}