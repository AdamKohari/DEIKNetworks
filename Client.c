#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 32
#define PORTNUM 1337
#define SENDERR	{printf("Error: kuldes sikertelen!\n"); exit(1);}
#define RCVERR {printf("Error: fogadas sikertelen!\n"); exit(1);}

int main(int argc, char** arguments)
{
	int sock;
	int flag;
	unsigned int server_size;
	int on;
	char data[BUFSIZE];
	struct sockaddr_in server;
	char inputline[BUFSIZE];
	int roundNum = 1;
	int maxrounds;
	char playerName[BUFSIZE];

	on = 1;
	flag = 0;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argc==1?"127.0.0.1":arguments[1]);
	server.sin_port = htons(PORTNUM);
	server_size = sizeof server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		printf("Error: socket letrehozasa sikertelen!\n");
 		exit(1);
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (int *)&on, sizeof on);
   	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (int *)&on, sizeof on);

   	if (connect(sock, (struct sockaddr *) &server, server_size) < 0)
   	{
   		printf("Error: csatlakozasi hiba!\n");
   		exit(1);
   	}

   	printf("Csatlakoztatva.\n");

   	if (recv(sock, playerName, BUFSIZE, flag) < 0)
   		RCVERR;

   	if (playerName[0] == 'A')
   		printf("Varakozas a masik jatekos csatlakozasara...\n\n");
   	else
   		printf("\n");

   	if (recv(sock, data, BUFSIZE, flag) < 0)
   		RCVERR;

   	maxrounds = atoi(data);
   	printf("A korok szama: %d\n", maxrounds);

   	if (playerName[0] == 'B')
   		printf("Varj, amig ellenfeled valaszt...\n");
   	

   	while(1)
   	{
   		if (recv(sock, data, BUFSIZE, flag) < 0)
   			RCVERR;

   		if (!strcmp(data, "YOURTURN"))
   		{
   			INPUT:
   			printf("--> %d. kor: Te jossz! Valassz targyat: ", roundNum);
   			scanf("%s", inputline);
   			if (!strcmp(inputline, "ko") || !strcmp(inputline, "papir") || !strcmp(inputline, "ollo") || !strcmp(inputline, "feladom"))
   			{
   				if (send(sock, inputline, BUFSIZE, flag) < 0)
   					SENDERR;
   				if (!strcmp(inputline, "feladom"))
   				{
   					printf("\n## FELADTAD A JATEKOT! ##\n");
   					break;
   				}
   			}
   			else
   			{
   				printf("Ervenytelen parancs!\n");
   				goto INPUT;
   			}
   			roundNum++;
   			if (playerName[0] == 'A')
   				printf("Varj, amig ellenfeled valaszt...\n");
   		}

   		if (playerName[0] == 'A')
   		{

   			if (!strcmp(data, "B_SURR"))
   			{
   				printf("\n## AZ ELLENFELED FELADTA! ##\n");
   				break;
   			}


   			if (!strcmp(data, "A_WON_RD"))
   				printf("Megnyerted a kort!\n");
   			if (!strcmp(data, "B_WON_RD"))
   				printf("Elvesztetted a kort!\n");
   			if (!strcmp(data, "TIE_RD"))
   				printf("Dontetlen kor!\n");


   			if (!strcmp(data, "A_WON_GM"))
   			{
   				printf("\n## MEGNYERTED A JATEKOT! ##");
   				if (recv(sock, data, BUFSIZE, flag) < 0)
   					RCVERR;
   				printf(" A vegeredmeny: %s\n", data);
   				break;
   			}
   			if (!strcmp(data, "B_WON_GM"))
   			{
   				printf("\n## ELVESZTETTED A JATEKOT! ##");
   				if (recv(sock, data, BUFSIZE, flag) < 0)
   					RCVERR;
   				printf(" A vegeredmeny: %s\n", data);
   				break;
   			}
   			if (!strcmp(data, "TIE_GM"))
   			{
   				printf("\n## DONTETLEN JATEK! ##");
   				if (recv(sock, data, BUFSIZE, flag) < 0)
   					RCVERR;
   				printf(" A vegeredmeny: %s\n", data);
   				break;
   			}
   		}
   		else
   		{
   			if (!strcmp(data, "A_SURR"))
   			{
   				printf("\n## AZ ELLENFELED FELADTA! ##\n");
   				break;
   			}

   			if (!strcmp(data, "A_WON_RD"))
   			{
   				printf("Elvesztetted a kort!\n");
   				if (roundNum-1 != maxrounds) printf("Varj, amig ellenfeled valaszt...\n");
   			}
   			if (!strcmp(data, "B_WON_RD"))
   			{
   				printf("Megnyerted a kort!\n");
   				if (roundNum-1 != maxrounds)printf("Varj, amig ellenfeled valaszt...\n");
   			}
   			if (!strcmp(data, "TIE_RD"))
   			{
   				printf("Dontetlen kor!\n");
   				if (roundNum-1 != maxrounds) printf("Varj, amig ellenfeled valaszt...\n");
   			}


   			if (!strcmp(data, "A_WON_GM"))
   			{
   				printf("\n## ELVESZTETTED A JATEKOT! ##");
   				if (recv(sock, data, BUFSIZE, flag) < 0)
   					RCVERR;
   				printf(" A vegeredmeny: %s\n", data);
   				break;
   			}
   			if (!strcmp(data, "B_WON_GM"))
   			{
   				printf("\n## MEGNYERTED A JATEKOT! ##");
   				if (recv(sock, data, BUFSIZE, flag) < 0)
   					RCVERR;
   				printf(" A vegeredmeny: %s\n", data);
   				break;
   			}
   			if (!strcmp(data, "TIE_GM"))
   			{
   				printf("\n## DONTETLEN JATEK! ##");
   				if (recv(sock, data, BUFSIZE, flag) < 0)
   					RCVERR;
   				printf(" A vegeredmeny: %s\n", data);
   				break;
   			}
   		}
   	}

   	close(sock);
   	return 0;
}
