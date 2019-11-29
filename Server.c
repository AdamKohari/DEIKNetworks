#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFSIZE 32
#define PORTNUM 1337
#define CQL     2

#define SENDTO_A {if (send(socketA, dataOut, BUFSIZE, flag) < 0){printf("Error: kuldes sikertelen!\n"); exit(1);}}

#define RCVFROM_A {if (recv(socketA, dataA, BUFSIZE, flag) < 0){printf("Error: fogadas sikertelen!\n"); exit(1);}}
#define SENDTO_B {if (send(socketB, dataOut, BUFSIZE, flag) < 0){printf("Error: kuldes sikertelen!\n"); exit(1);}}
#define RCVFROM_B {if (recv(socketB, dataB, BUFSIZE, flag) < 0){printf("Error: fogadas sikertelen!\n"); exit(1);}}

int generalSocket;
int socketA, socketB;
int connectedClients = 0;
int numberOfGames;
int aPoints = 0;
int bPoints = 0;
int roundResult; //1 = A nyert, 2 = B nyert, 0 = dontetlen
int surrended = 0;

void stop()
{
	close(generalSocket); close(socketA); close(socketB);
 	printf("A szerver leallt.\n");
 	return;
}

int main(int argc, char** arguments)
{
 	int flag;
 	unsigned int server_size;
 	unsigned int clientA_size;
 	unsigned int clientB_size;
 	int on;
 	char dataA[BUFSIZE];
 	char dataB[BUFSIZE];
 	char dataOut[BUFSIZE];
 	struct sockaddr_in server;
 	struct sockaddr_in clientA;
 	struct sockaddr_in clientB;

 	numberOfGames = atoi(arguments[1]);

 	on = 1;
 	flag = 0;
 	server.sin_family =	AF_INET;
 	server.sin_addr.s_addr = INADDR_ANY;
 	server.sin_port = htons(PORTNUM);
 	server_size = sizeof server;
 	clientA_size = sizeof clientA;
 	clientB_size = sizeof clientB;

 	generalSocket = socket(AF_INET, SOCK_STREAM, 0);
 	if (generalSocket < 0)
 	{
 		printf("Error: socket letrehozasa sikertelen!\n");
 		exit(1);
 	}
 	setsockopt(generalSocket, SOL_SOCKET, SO_REUSEADDR, (int *)&on, sizeof on);
   	setsockopt(generalSocket, SOL_SOCKET, SO_KEEPALIVE, (int *)&on, sizeof on);
 	
 	if (bind(generalSocket, (struct sockaddr *) &server, server_size) < 0)
 	{
 		printf("Error: sikertelen binding!\n");
 		exit(1);
 	}

 	if (listen(generalSocket, CQL) < 0)
 	{
 		printf("Error: sikertelen listening!\n");
 		exit(1);
 	}

 	printf("A szerver keszen all a kliensek csatlakozasara\n");

 	while(1)
 	{
 		if (connectedClients == 0)
 		{
 			socketA = accept(generalSocket, (struct sockaddr *) &clientA, &clientA_size);
 			if (socketA < 0)
 			{
 				printf("Error: hiba a csatlakozasi keres elfogadasakor!\n");
 				exit(1);
 			}

 			printf("Kliens 'A' sikeresen csatlakozott!\n");
 			connectedClients++;
 			strcpy(dataOut, "A");
 			SENDTO_A;

 		}

 		if (connectedClients == 1)
 		{
 			socketB = accept(generalSocket, (struct sockaddr *) &clientB, &clientB_size);

 			if (socketB < 0)
 			{
 				printf("Error: hiba a csatlakozasi keres elfogadasakor!\n");
 				exit(1);
 			}

 			printf("Kliens 'B' sikeresen csatlakozott!\n");
 			connectedClients++;
 			strcpy(dataOut, "B");
 			SENDTO_B;
 		}

 		if (connectedClients == 2)
 			break;
 	}
 	

 	sprintf(dataOut, "%d", numberOfGames);

 	SENDTO_A;
 	SENDTO_B;

 	for (int i = 0; i<numberOfGames; i++)
 	{
 		strcpy(dataOut, "YOURTURN");
 		SENDTO_A;

 		RCVFROM_A;

 		if (!strcmp(dataA, "feladom"))
 		{
 			surrended = 1;
 			strcpy(dataOut, "A_SURR");
 			SENDTO_B;
 			break;
 		}

 		strcpy(dataOut, "YOURTURN");
 		SENDTO_B;

 		RCVFROM_B;

 		
 		if (!strcmp(dataB, "feladom"))
 		{
 			strcpy(dataOut, "B_SURR");
 			surrended = 1;
 			SENDTO_A;
 			break;
 		}


 		if ( (!strcmp(dataA, "ko") && !strcmp(dataB, "ko")) || (!strcmp(dataA, "papir") && !strcmp(dataB, "papir")) || (!strcmp(dataA, "ollo") && !strcmp(dataB, "ollo")) )
 			roundResult = 0;

 		else if ( (!strcmp(dataA, "ko") && !strcmp(dataB, "papir")) || (!strcmp(dataA, "ollo") && !strcmp(dataB, "ko")) || (!strcmp(dataA, "papir") && !strcmp(dataB, "ollo")) )
 			roundResult = 2;
 		else
 			roundResult = 1;

 		
 		if (roundResult == 0)
 		{
 			strcpy(dataOut, "TIE_RD");
 			SENDTO_A;
 			SENDTO_B;
 		}
 		if (roundResult == 1)
 		{
 			strcpy(dataOut, "A_WON_RD");
 			SENDTO_A;
 			SENDTO_B;
 			aPoints++;
 		}
 		if (roundResult == 2)
 		{
 			strcpy(dataOut, "B_WON_RD");
 			SENDTO_A;
 			SENDTO_B;
 			bPoints++;
 		}

 	}

 	if (surrended)
 	{
 		stop();
 		return 0;
 	}

 	if (aPoints < bPoints)
 	{
 		strcpy(dataOut, "B_WON_GM");
 		SENDTO_A;
 		SENDTO_B;
 		sprintf(dataOut, "%d-%d", aPoints, bPoints);
 		SENDTO_A;
 		SENDTO_B;
 	} 
 	else if (aPoints > bPoints)
 	{
 		strcpy(dataOut, "A_WON_GM");
 		SENDTO_A;
 		SENDTO_B;
 		sprintf(dataOut, "%d-%d", aPoints, bPoints);
 		SENDTO_A;
 		SENDTO_B;
 	}
 	else
 	{
 		strcpy(dataOut, "TIE_GM");
 		SENDTO_A;
 		SENDTO_B;
 		sprintf(dataOut, "%d-%d", aPoints, bPoints);
 		SENDTO_A;
 		SENDTO_B;
 	}

 	stop();
 	return 0;
 }
