/*
** client.c -- client to the battle server game, connects to the server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "9034" // the port client will be connecting to 
#define STDIN 0 //STDIN
#define MAXDATASIZE 2048 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
		return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes, msgbytes;
	int AcceptFlag = 0;  
	char buf[MAXDATASIZE];
	char msg[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv, i;
	char s[INET6_ADDRSTRLEN];
	fd_set master;
	fd_set read_fds;
	int fdmax;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	FD_SET(sockfd, &master);
	FD_SET(STDIN, &master);
	fdmax = sockfd;
	fprintf(stderr, "Welcome to Battle-Server\n");
	fprintf(stderr, "When you are ready to proceed type 'Accept' and hit enter\n");
	//main loop
	for(;;)
	{
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			exit(4);
		}
		for(i = 0; i<=fdmax; i++){
			if (FD_ISSET(i, &read_fds)) {
				if(i == sockfd){
					numbytes = recv(sockfd, buf, MAXDATASIZE, 0);
					printf("%s\n",buf);
					if(numbytes == 256)
					{
						FD_SET(STDIN, &master);
					}
					if(numbytes <= 0){close(sockfd);}
				}
				else
				{
					memset(msg, 0, sizeof(msg));
					msgbytes = read(STDIN, msg, sizeof(msg));
					msg[msgbytes] = '\0';
					while(strcmp(msg, "Accept\n") != 0 && AcceptFlag != 1)
					{
						fprintf(stderr, "Please type 'Accept' to proceed: \n");
						memset(msg, 0, sizeof(msg));
						msgbytes = read(STDIN, msg, sizeof(msg));
					}
					if(strcmp(msg, "Attack\n") == 0 || strcmp(msg, "Defend\n") == 0 || strcmp(msg, "Heal\n") == 0)
					{
						fprintf(stderr, "\nPlease Wait for Other Players\n");
						FD_CLR(STDIN, &master);
					}
					if (AcceptFlag != 1)
					{
					fprintf(stderr, "- - - - - - - \nLogin Success |\n- - - - - - -\n");
					fprintf(stderr, "Here are your options:\nAttack\nGuard\nHeal\nOptions(if you forget)\n");
					fprintf(stderr, "\nYou have been");
					}
					if(strcmp(msg, "Options\n") == 0)
					{
						fprintf(stderr, "- - - - - \nRemember here are your options:\nAttack\nGuard\nHeal\n- - - - - \n\n");
					}
					AcceptFlag = 1;
					send(sockfd, msg, msgbytes, 0);
					if(msgbytes <= 1){close(sockfd);}
				}
			}
		}
	}
	return 0;
}
