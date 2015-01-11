#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>


int main(int argc, char const *argv[])
{
	int s,res;
	struct sockaddr_in ad_dest;
	char msg[50];
	s = socket(AF_INET,SOCK_STREAM,0);
	ad_dest.sin_family = AF_INET;
	ad_dest.sin_port = htons(atoi(argv[1]));
	struct hostent *h = gethostbyname(argv[2]);
	memcpy(&(ad_dest.sin_addr.s_addr), h->h_addr, h->h_length);
	memset(&(ad_dest.sin_zero), '\0', 8);
	res = connect(s,(struct sockaddr*)&ad_dest,sizeof(ad_dest));
	if (res==-1)
	{
		perror("bind");
		return 0;
	}
	write(s,msg,strlen(msg)+1);
	read(s,msg,sizeof(msg));
	printf("%s\n",msg);
	close(s);
	return 0;
}