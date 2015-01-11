#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
	int s,res,s_srv;
	socklen_t taille;
	struct sockaddr_in ad_local, ad_emet;
	char msg[50];
	s = socket(AF_INET, SOCK_STREAM, 0);
	ad_local.sin_port = htons(atoi(argv[1]));
	ad_local.sin_family = AF_INET;
	ad_local.sin_addr.s_addr = INADDR_ANY;
	memset(ad_local.sin_zero,0,8);
	char hostname[50];

	res = bind(s,(struct sockaddr *)&ad_local, sizeof(ad_local));
	if (res==-1)
	{
		perror("bind");
		return 0;
	}
	gethostname(hostname,sizeof(hostname));
	printf("%s\n",hostname);
	while(1){
		listen(s,5);
		taille = sizeof(ad_emet);
		s_srv = accept(s,(struct sockaddr*)&ad_emet,&taille);
		read(s_srv,&msg,sizeof(msg));
		printf("msg reçu = %s\n", msg);
		strcpy(msg,"ok");
		write(s_srv,msg,strlen(msg)+1);
		close(s_srv);
	}
	return 0;
}