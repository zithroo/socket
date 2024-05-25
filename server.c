#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)


void handle_client(int connfd);

int main () {	
	struct sockaddr_in myaddr, client_addr;	
	int sockfd, streamfd, port, status, addr_size;	
	char str_buf[100];		
	bzero(&myaddr, sizeof(myaddr));	
	myaddr.sin_family = PF_INET;	
	myaddr.sin_port = htons(1234);	
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0) ERR_EXIT("socket");
	if(bind(sockfd, (struct sockaddr *) &myaddr, sizeof(struct sockaddr_in)) < 0) {
		ERR_EXIT("bind");
	}
	if(listen(sockfd, 10) < 0) {
		ERR_EXIT("listen");
	}
	
	addr_size = sizeof(client_addr);	
	while(1){
	    streamfd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);	
	
	    handle_client(streamfd);     //Call the procedure you wish to perform		
	
	    close(streamfd);	
	}
    return 0;
}

void handle_client(int connfd) {

}