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

int main(int argc, char const *argv[]) {

	char message[255], myScore[255];
	int server, portNumber, score;
	socklen_t len;
	struct sockaddr_in servAdd;

 	if(argc != 4){
  		printf("Call model:%s <IP> <Port#> <Player Name>\n",argv[0]);
  		exit(0);
	}
	
	if((server = socket(AF_INET, SOCK_STREAM, 0))<0){
  		fprintf(stderr, "Cannot create socket\n");
		exit(1);
	}
	
	servAdd.sin_family = AF_INET;
	sscanf(argv[2], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);
	
	if(inet_pton(AF_INET,argv[1],&servAdd.sin_addr) < 0){
		fprintf(stderr, " inet_pton() has failed\n");
		exit(2);
	}
	
	if(connect(server,(struct sockaddr *)&servAdd,sizeof(servAdd)) < 0){
 		fprintf(stderr, "connect() has failed, exiting\n");
		exit(3);
	}

	printf("%s - Player Connected to the Server!!\n", argv[3]);
	
	write(server, argv[3], strlen(argv[3])+1);

	while(1){
		
		if(read(server, message, 255) < 0) {
   			fprintf(stderr, "read() error\n");
			exit(3);
		}
		
		if((strcmp(message,"Your turn now") == 0)){

			printf("%s in the game\n",message);
			score = (int)time(NULL)%6 + 1;
			sprintf(myScore, "%d", score);
			printf("\n%s - SCORE NOW: %s\n\n",argv[3],myScore);
			strcpy(message,myScore);
			
			write(server, message, strlen(message)+1);
		}
		
		if((strcmp(message,"Game over") == 0)){

			printf("WINNER!!\n\n");
			close(server);
			exit(0);
		}
		
		if((strcmp(message,"Game over:") == 0)){

			printf("LOSER!\n\n");
			close(server);
			exit(0);
		}
	}
}


