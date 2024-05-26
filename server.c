#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)
#define BUFFSIZE 256
enum req_type { DNS=1, QUERY, QUIT, ERR};

void handle_client(int connfd);
void handle_DNS(int connfd, char *str_buf);
void handle_QUERY(int connfd, char *str_buf);
void handle_QUIT(int connfd, char *str_buf);
void handle_ERR(int connfd, char *str_buf);


int main () {	
	struct sockaddr_in myaddr, client_addr;	
	int sockfd, connfd, port, status, addr_size, option = 1;	
	bzero(&myaddr, sizeof(myaddr));	
	myaddr.sin_family = PF_INET;	
	myaddr.sin_port = htons(1234);	
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0) ERR_EXIT("socket");
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if(bind(sockfd, (struct sockaddr *) &myaddr, sizeof(struct sockaddr_in)) < 0) {
		ERR_EXIT("bind");
	}
	if(listen(sockfd, 10) < 0) {
		ERR_EXIT("listen");
	}
	printf("Server listening on port %d ...\n", ntohs(myaddr.sin_port));

	addr_size = sizeof(client_addr);	
	while(1) {
	    connfd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);	
		if(connfd < 0) {
			ERR_EXIT("accept");
		}
		printf("Connected %s, Waiting message ...\n", inet_ntoa(client_addr.sin_addr));

	    handle_client(connfd);     // Call the procedure you wish to perform		
		
		printf("Disconnected %s \n", inet_ntoa(client_addr.sin_addr));
	    close(connfd);
			
	}
    return 0;
}

void handle_client(int connfd) {
	char str_buf[BUFFSIZE];
	int requirement;
	
	while(1) {
		memset(str_buf, 0, sizeof(str_buf));
		requirement = 4;
		strncpy(str_buf, "What's your requirement? 1.DNS 2.QUERY 3.QUIT : ", BUFFSIZE-1);
		if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
			ERR_EXIT("writting to socket");
		}

		// Read requirement
		if(read(connfd, str_buf, BUFFSIZE) < 0) {
			ERR_EXIT("reading from socket");
		}
		if(sscanf(str_buf, "%d", &requirement) != 1) {
			requirement = 4;
			while (getchar() != '\n'); // flush
		}
		printf("reqirment = %d\n", requirement);
		// handle requirement
		switch (requirement)
		{
		case DNS:
			handle_DNS(connfd, str_buf);
		break;
		case QUERY:
			handle_QUERY(connfd, str_buf);
		break;
		case QUIT:
			handle_QUIT(connfd, str_buf);
			return;
		break;
	
		default:
			handle_ERR(connfd, str_buf);
		break;
		}
	}

}

void handle_DNS(int connfd, char *str_buf) {
	printf("Request: DNS\n");
	// return status code
	if(write(connfd, "1", 2) < 0) {
		ERR_EXIT("writting to socket");
	}

	strncpy(str_buf, "Input URL address : ", BUFFSIZE-1);
	if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
		ERR_EXIT("writting to socket");
	}

	if(read(connfd, str_buf, BUFFSIZE) < 0) {
		ERR_EXIT("reading from socket");
	}
	printf("Request: url = %s\n", str_buf);

	strncpy(str_buf, "address get from domain name : \n", BUFFSIZE-1);
	if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
		ERR_EXIT("writting to socket");
	}
}

void handle_QUERY(int connfd, char *str_buf) {
	printf("Request: QUERY\n");
	// return status code
	if(write(connfd, "2", 2) < 0) {
		ERR_EXIT("writting to socket");
	}

	FILE *pFile;
	pFile = fopen("query.txt", "r");
	if(pFile == NULL) {
		ERR_EXIT("Open failure: query.txt");
	}

	strncpy(str_buf, "Input student ID : ", BUFFSIZE-1);
	if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
		ERR_EXIT("writting to socket");
	}

	if(read(connfd, str_buf, BUFFSIZE) < 0) {
		ERR_EXIT("reading from socket");
	}
	printf("Request: student ID = %s\n", str_buf);

	char line[128];
	int flag = 0;
	while(fgets(line, sizeof(line), pFile)) {
		line[strcspn(line, "\n")] = 0;
		char *token = strtok(line, " \t");
		if(token == NULL) {
			ERR_EXIT("Invalid line format");
		}

        char *id = token, *email = strtok(NULL, " \t");
		if(email == NULL) {
			ERR_EXIT("Email parsing");
		}

		if(strcmp(id, str_buf) == 0) {
			flag = 1;
			sprintf(str_buf, "Email get from server : %s\n", email);
			if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
			ERR_EXIT("writting to socket");
			}
			break;
		}
	}

	if(!flag) {
		strncpy(str_buf, "No such student ID\n", BUFFSIZE-1);
		if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
			ERR_EXIT("writting to socket");
		}
	}

	fclose(pFile);
}

void handle_QUIT(int connfd, char *str_buf) {
	printf("Request: Quit\n");
	// return status code
	if(write(connfd, "3", 2) < 0) {
		ERR_EXIT("writting to socket");
	}
}

void handle_ERR(int connfd, char *str_buf) {
	printf("Reject: requirement out of range\n");
	// return status code
	if(write(connfd, "4", 2) < 0) {
		ERR_EXIT("writting to socket");
	}
}