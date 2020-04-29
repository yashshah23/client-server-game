/*
COMP-8567 
Advanced Systems Programming

Final Project

Team Members:
Yash Shah - 110008515
Kaushal Tank - 105143633
*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void multiPlayers(int,int);
void scoreValidate(int[],int,int,char[],char[]);


/*
Checks score amd declares winner among the players playing the game
*/
void scoreValidate(int totalScore[], int client1, int client2, char player1[], char player2[]){

	char message[255];
	int maxScore = 100;

	
	if (totalScore[0] >= maxScore){

		strcpy(message, "Game over");
		write(client1, message, strlen(message)+1);

		strcpy(message, "Game over:");
		write(client2, message, strlen(message)+1);

		close(client1);
		close(client2);

		printf("\n%s WINNER!!\n",player1);
		printf("\nWaiting for next GAME to start!\n");
		exit(0);
	}

	
	if(totalScore[1] >= maxScore){

		strcpy(message, "Game over");
		write(client2, message, strlen(message)+1);

		strcpy(message, "Game over:");
		write(client1, message, strlen(message)+1);

		close(client1);
		close(client2);

		printf("\n%s WINNER!!\n",player2);
		printf("\nWaiting for next GAME to start!\n");
		exit(0);
	}
}


/*
It will act as a refree for the game played and will be called by the child processes for the function call
*/
void multiPlayers(int client1, int client2){

	char message[255], player1[100], player2[100];
	int totalScore[2], obtainedScore;

	
	if(!read(client1, player1, 100)){
    		close(client1);
    		fprintf(stderr,"Unable to receive data from client\n");
		exit(0);  
	}

	
	if(!read(client2, player2, 100)){
    		close(client1);
    		fprintf(stderr,"Unable to receive data from client\n");
		exit(0);
	}

	
	for(int i = 0; i < 2; i++)
		totalScore[i] = 0;

	while(1){
		
		strcpy(message,"Your turn now");
		write(client1, message, strlen(message)+1);

		
		if(!read(client1, message, 255)){
    			close(client1);
    			fprintf(stderr,"Unable to receive data from client\n");
			exit(0);
		}

		sscanf(message,"%d",&obtainedScore);
		totalScore[0] += obtainedScore;
		printf("%s - Total Score till now = %d\n\n",player1,totalScore[0]);
		sleep(1);

		strcpy(message, "Your turn now");
		write(client2, message, strlen(message)+1);

		if(!read(client2, message, 255)){
    			close(client2);
    			fprintf(stderr,"Unable to receive data from client\n");
			exit(0);
		}

		sscanf(message,"%d",&obtainedScore);
		totalScore[1] += obtainedScore;
		printf("%s - Total Score till now = %d\n\n",player2,totalScore[1]);
		sleep(1);

		scoreValidate(totalScore, client1, client2, player1, player2);
	}
}



int main(int argc, char const *argv[])
{
	int sd, client1, client2, portNumber, pid;
	socklen_t len;
	struct sockaddr_in servAdd;

 	if(argc != 2){
    		printf("Enter port arguement: %s <Port #>\n", argv[0]);
    		exit(0);
	}

	//initial Socket creation
	if ((sd=socket(AF_INET,SOCK_STREAM,0)) < 0) {
   		fprintf(stderr, "Cannot create socket\n");
		exit(1);
	}

	//IP address and port number configuration
	servAdd.sin_family = AF_INET;
	servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	sscanf(argv[1], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);

	
	bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));
	listen(sd, 5);

	printf("\nWaiting for players response!\n");

	while(1){

		
		client1=accept(sd,(struct sockaddr*)NULL,NULL);
    		printf("Player 1 Connected\n");

		client2=accept(sd,(struct sockaddr*)NULL,NULL);
		printf("Player 2 Connected\n");

		printf("\nTwo players connected, GAME ON!\n");

		
		pid = fork();
		if(pid == 0)
			multiPlayers(client1, client2);

		close(client1);
		close(client2);
	}

	return 0;
}
