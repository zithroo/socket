#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)
#define BUFFSIZE 128
enum req_type { DNS=1, QUERY, QUIT, ERR};


void Server_Request_Procedure(int sockfd);
void dns(int sockfd, char *str_buf);
void query(int sockfd, char *str_buf);
void quit(int sockfd, char *str_buf);
void err(int sockfd, char *str_buf);



int main () {	
	struct sockaddr_in server_addr;	
	int sockfd, status;	
	//setup the server address	
	server_addr.sin_family = PF_INET;	
	server_addr.sin_port = htons(1234);	
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	//connect to the server	
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) ERR_EXIT("socket");
	if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) < 0) {
		ERR_EXIT("connect");
	}
	Server_Request_Procedure(sockfd); //Call procedure you wish to perform


	close(sockfd);	
	return 0;
}

void Server_Request_Procedure(int sockfd) {
	char str_buf[BUFFSIZE];
	int requirement, status = 0;
	while(1) {
		// What's your requirement?
		if(read(sockfd, str_buf, BUFFSIZE) < 0) {
			ERR_EXIT("reading from socket");
		}
		printf("%s", str_buf);
		
		if(scanf("%d", &requirement) != 1) {
			requirement = 4;
			while (getchar() != '\n'); // flush
		}
		sprintf(str_buf, "%d", requirement);
		if(write(sockfd, str_buf, strlen(str_buf)+1) < 0) {
			ERR_EXIT("writting to socket");
		}
		
		// get status code
		if(read(sockfd, str_buf, 2) < 0) {
			ERR_EXIT("reading from socket");
		}
		sscanf(str_buf, "%d", &status);
		// handle responese
		switch (status)
		{
		case DNS:
			dns(sockfd, str_buf);
		break;
		
		case QUERY:
			query(sockfd, str_buf);
		break;
		
		case QUIT:
			quit(sockfd, str_buf);
		return;

		default:
			err(sockfd, str_buf);
		break;
		}
	}
}

void dns(int sockfd, char *str_buf) {
	// Input URL address : 
	if(read(sockfd, str_buf, BUFFSIZE) < 0) {
		ERR_EXIT("reading from socket");
	}
	printf("%s", str_buf);
	// URL
	scanf("%s", str_buf);
	if(write(sockfd, str_buf, strlen(str_buf)+1) < 0) {
		ERR_EXIT("writting to socket");
	}
	// Domain name
	if(read(sockfd, str_buf, BUFFSIZE) < 0) {
		ERR_EXIT("reading from socket");
	}
	printf("%s", str_buf);
}

void query(int sockfd, char *str_buf) {
	int student_ID;
	// Input student ID : 
	if(read(sockfd, str_buf, BUFFSIZE) < 0) {
		ERR_EXIT("reading from socket");
	}
	printf("%s", str_buf);

	if(scanf("%d", &student_ID) != 1) {
		student_ID = 0;
		while (getchar() != '\n'); // flush
	}
	sprintf(str_buf, "%d", student_ID);
	if(write(sockfd, str_buf, strlen(str_buf)+1) < 0) {
		ERR_EXIT("writting to socket");
	}

	if(read(sockfd, str_buf, BUFFSIZE) < 0) {
		ERR_EXIT("reading from socket");
	}
	printf("%s", str_buf);
}

void quit(int sockfd, char *str_buf) {
}

void err(int sockfd, char *str_buf) {
	printf("Reject by server : err\n\n");
}