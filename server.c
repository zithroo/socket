#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>


#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)
#define BUFFSIZE 128
enum req_type { DNS=1, QUERY, QUIT, ERR};

void handle_client(int connfd);
void handle_DNS(int connfd, char *str_buf);
void handle_QUERY(int connfd, char *str_buf, FILE *pFile);
void handle_QUIT(int connfd, char *str_buf);
void handle_ERR(int connfd, char *str_buf);


int main () {	
	struct sockaddr_in myaddr, client_addr;	
	int sockfd, connfd, port, status, addr_size, option = 1, pid;	
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
		pid = fork();
		if(pid < 0) {
			ERR_EXIT("fork");
		}
		if(pid == 0) {
			// Child proccess, handle client req
			close(sockfd);
			printf("Connected %s, pid = %ld\n", inet_ntoa(client_addr.sin_addr), (long) getpid());
	    	handle_client(connfd);     // Call the procedure you wish to perform		
			printf("Disconnected %s, pid = %ld\n", inet_ntoa(client_addr.sin_addr), (long) getpid());
	    	close(connfd);
			exit(0);
		}
		else {
			// Parent proccess, accept connection
			close(connfd);
		}
	}
    return 0;
}

void handle_client(int connfd) {
	char str_buf[BUFFSIZE];
	int requirement, quit_flag = 1;
	memset(str_buf, 0, sizeof(str_buf));
	requirement = 4;
	
	FILE *pFile;
	pFile = fopen("query.txt", "r");
	if(pFile == NULL) {
		ERR_EXIT("Open failure: query.txt");
	}

	while(quit_flag) {
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
		// handle requirement
		switch (requirement)
		{
		case DNS:
			handle_DNS(connfd, str_buf);
		break;

		case QUERY:
			handle_QUERY(connfd, str_buf, pFile);
		break;
		
		case QUIT:
			handle_QUIT(connfd, str_buf);
			quit_flag = 0;
		break;
	
		default:
			handle_ERR(connfd, str_buf);
		break;
		}
		// Prevent server write -> read when client read (race)
		// But there is always possible that delay > 0.3 sec QQ
		sleep(0.3);
		// printf("done\n");
	}
	fclose(pFile);
}

void handle_DNS(int connfd, char *str_buf) {
	printf("Request: DNS, pid = %ld\n", (long) getpid());
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
	printf("Request: url = %s, pid = %ld\n", str_buf, (long) getpid());

	struct addrinfo hints, *p, *listp;
	int status, flags;
	char domain[BUFFSIZE];
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if((status = getaddrinfo(str_buf, NULL, &hints, &listp)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		strncpy(str_buf, "No such DNS\n\n", BUFFSIZE-1);
		if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
			ERR_EXIT("writing to socket");
		}
		return;
	}
	flags = NI_NUMERICHOST;
	for(p = listp; p; p = p->ai_next) {
		getnameinfo(p->ai_addr, p->ai_addrlen, domain, BUFFSIZE, NULL, 0, flags);
		// printf("%s\n", domain);
	}

	freeaddrinfo(listp);

	sprintf(str_buf, "address get from domain name : %s\n\n", domain);
	if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
		ERR_EXIT("writting to socket");
	}
}

void handle_QUERY(int connfd, char *str_buf, FILE *pFile) {
	printf("Request: QUERY, pid = %ld\n", (long) getpid());
	// return status code
	if(write(connfd, "2", 2) < 0) {
		ERR_EXIT("writting to socket");
	}

	strncpy(str_buf, "Input student ID : ", BUFFSIZE-1);
	if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
		ERR_EXIT("writting to socket");
	}

	if(read(connfd, str_buf, BUFFSIZE) < 0) {
		ERR_EXIT("reading from socket");
	}
	printf("Request: student ID = %s, pid = %ld\n", str_buf, (long) getpid());

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
			sprintf(str_buf, "Email get from server : %s\n\n", email);
			if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
			ERR_EXIT("writting to socket");
			}
			break;
		}
	}

	if(!flag) {
		strncpy(str_buf, "No such student ID\n\n", BUFFSIZE-1);
		if(write(connfd, str_buf, strlen(str_buf)+1) < 0) {
			ERR_EXIT("writting to socket");
		}
	}

}

void handle_QUIT(int connfd, char *str_buf) {
	printf("Request: Quit, pid = %ld\n", (long) getpid());
	// return status code
	if(write(connfd, "3", 2) < 0) {
		ERR_EXIT("writting to socket");
	}
}

void handle_ERR(int connfd, char *str_buf) {
	printf("Reject: requirement out of range, pid = %ld\n", (long) getpid());
	// return status code
	if(write(connfd, "4", 2) < 0) {
		ERR_EXIT("writting to socket");
	}
}