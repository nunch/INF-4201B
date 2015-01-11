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

int main(int argc, char const *argv[])
{
	char* msg = (char*) malloc(50*sizeof(char));
	int horloge;
	int machine;
	char* msg2 = (char*) malloc(50*sizeof(char));
	sprintf(msg,"58|126|encule de merde");
	sscanf(msg,"%d|%d|%[^\n]",&machine,&horloge,msg2);
	printf("message : %s\n", msg);
	printf("%d|%d|%s\n", machine,horloge,msg2);
	return 0;
}