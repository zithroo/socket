#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>


#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)

void Server_Request_Procedure(int sockfd);

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
    
}