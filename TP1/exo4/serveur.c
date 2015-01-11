#include <netinet/in.h> /* pour protocole ds cas AF_INET */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


int main(int argc, char const *argv[]) {
	if(argc != 3) {
		printf("2 paramètres attendus\n");
		printf("Les ports sur lequels il va attendre les connexions des clients.\n");
		printf("Il répondra normalement aux requêtes HTTP sur le premier port,\n");
		printf("et retournera son fichier de log si un client s’y connecte par le second port.\n");
		return -1;
	}

	// On déclare les variables
	int s_web, s_log,res,s_srv, i;
	int fd_log, fd_file;
	char LOG[5000];
	char file[20];
	char file_in[5000];
	time_t secondes;
	struct tm instant;
	char date[50];
	socklen_t taille;

	// On récupère les numéros de port sur forme de 'int'
	int portWeb = atoi(argv[1]);
	int portLog = atoi(argv[2]);

	struct sockaddr_in ad_local_web, ad_local_log, ad_emet;
	char msg[5000];

	s_log = socket(AF_INET, SOCK_STREAM, 0);
	if(s_log == -1) { perror("socket"); return -1; }

	s_web = socket(AF_INET, SOCK_STREAM, 0);
	if(s_web == -1) { perror("socket"); return -1; }

	ad_local_log.sin_port = htons(portLog);
	ad_local_log.sin_family = AF_INET;
	ad_local_log.sin_addr.s_addr = INADDR_ANY;
	memset(ad_local_log.sin_zero,0,8);

	ad_local_web.sin_port = htons(portWeb);
	ad_local_web.sin_family = AF_INET;
	ad_local_web.sin_addr.s_addr = INADDR_ANY;
	memset(ad_local_web.sin_zero,0,8);

	char hostname[50];

	res = bind(s_log,(struct sockaddr *)&ad_local_log, sizeof(ad_local_log));
	if (res==-1)
	{
		perror("bind log");
		return 0;
	}


	res = bind(s_web,(struct sockaddr *)&ad_local_web, sizeof(ad_local_web));
	if (res==-1)
	{
		perror("bind web");
		return 0;
	}

	res = listen(s_web,1);
	if (res==-1)
	{
		perror("listen web");
		return 0;
	}
	res = listen(s_log,1);
	if (res==-1)
	{
		perror("listen web");
		return 0;
	}

	fd_set* ptr_lecture = (fd_set*) malloc(2*sizeof(fd_set));

	gethostname(hostname,sizeof(hostname));
	printf("%s\n",hostname);

	int result;

	while(1){
		FD_ZERO(ptr_lecture);
		FD_SET(s_log, ptr_lecture);
		FD_SET(s_web, ptr_lecture);

		result = select(s_web + 1, ptr_lecture, NULL, NULL, NULL);
		if (result<0)
		{
			perror("select");
			//return 0;
		}
		printf("select passé\n");

		if(FD_ISSET(s_web, ptr_lecture)){
			printf("s_web\n");
		    taille = sizeof(ad_emet);
		    s_srv = accept(s_web, (struct sockaddr *)&ad_emet, &taille);
		    printf("accept fait\n");

			fd_log = open("log_file", O_RDWR|O_CREAT);
			if (fd_log==-1)
			{
				perror("open");
				return 0;
			}
			while(read(fd_log,&msg[0], 1));
			printf("read log\n");
		    time(&secondes);
		    instant=*localtime(&secondes);
			sprintf(date,"%d/%d__%d:%d:%d", instant.tm_mday+1, instant.tm_mon+1, instant.tm_hour, instant.tm_min, instant.tm_sec);
			memset(msg,sizeof(msg),'\0');
			memset(file,sizeof(file),'\0');
			printf("temps près\n");
			read(s_srv,&msg,sizeof(msg));
			printf("read fait %s ou pas\n",msg);
			i = 4;
			while(msg[i]!=' '){
				file[i-4] = msg[i];
				i++;
			}
			file[i-3]='\0';
			memset(msg,sizeof(msg),'\0');
			//printf("%s %s %s\n", inet_ntoa(ad_emet.sin_addr), date, file);
			sprintf(msg,"%s %s %s\n", inet_ntoa(ad_emet.sin_addr), date, file);
			printf("%s\n", msg);
			write(fd_log, msg, strlen(msg));
			printf("écriture faite dans log\nfile : %s\n",file);
			memset(file_in,sizeof(file_in),'\0');
			close(fd_log);

			fd_file = open(file,O_RDONLY);
			if (fd_file==-1){
				char c[20];
				sprintf(c,"bad file");
				write(s_srv, c, strlen(c)+1);
				close(s_srv);
			}
			else{
				i=0;
				while(read(fd_file,&file_in[i], 1)){
					i++;
				}
				printf("read file\n");
				write(s_srv, file_in, strlen(file_in)+1);
				printf("reponse\n");
				close(s_srv);
			}
		} else if(FD_ISSET(s_log, ptr_lecture)){
			printf("s_log\n");	
		    taille = sizeof(ad_emet);
		    s_srv = accept(s_log, (struct sockaddr *)&ad_emet, &taille);
			fd_log = open("log_file", O_RDWR|O_CREAT);
			if (fd_log==-1)
			{
				perror("open");
				return 0;
			}
			read(fd_log,&LOG[0], sizeof(LOG));
			printf("%s\n", LOG);
			write(s_srv, LOG, strlen(LOG)+1);
			close(s_srv);
		}else{
			printf("mauvais\n");
		}
	}

	close(s_web);
	close(s_log);

	return 0;
}