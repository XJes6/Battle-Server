/*
** selectserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "Queue.h"

#define PORT "9034"   // port we're listening on
#define STDIN 0

typedef struct Monster
{
    char* Name;
    int HP;
    int PATK;
    
}Monster;

typedef struct Client
{
    char* Name;
    int HP;
    int PATK;
    int fd;
    
}Client;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{

		return &(((struct sockaddr_in*)sa)->sin_addr);

}

int randrnge(int lower, int upper)
{
    int i;
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[1024];    // buffer for client data
    char msg[1024];    // buffer for STDIN
    char resp[1024];   // buffer for responses
    int nbytes, msgbytes;

	char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv, clients;

	struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}
	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);
    FD_SET(STDIN, &master);
    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
					newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
                    }
                } else if (FD_ISSET(STDIN, &read_fds)) //SERVER TYPES
                {
                    memset(msg, 0, sizeof(msg));
                    msgbytes = read(STDIN, msg, sizeof(msg));
                    for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, msg, msgbytes, 0) == -1) {
                                        perror("send");
                                    }
                                    else {
                                        printf("sent\n");
                                    }
                                }
                            }
                        }
                }
                
                else
                {

                    printf("msgbytes = %i\n", msgbytes);
                    memset(buf, 0, sizeof(buf));
                    nbytes = recv(i, buf, sizeof(buf), 0);
                    printf("nbytes: %i\n", nbytes);
                    // handle data from a client
                    if (nbytes <= 1) { //CONNECTION KILLED
                        // got error or connection closed by client
                        if (nbytes <= 1) 
                        {
                            // connecton closed
                            printf("selectserver: socket %d hung up\n", i);
                        } 
                        else
                        {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else { 
                        // we got some data from a client //TODO Make a ifs for Accept, Attack, Guard, Heal
                        if (strcmp(buf, "Accept\n") == 0) //TODO Add a struct for Clients
                        {

                            //------------------------------
                            // DEVELOP ACCEPT FUNCTION
                            //------------------------------

                            memset(resp, 0, sizeof(resp));
                            strcpy(resp, "\n- - - - - - -\nAcknowledged |\n- - - - - - -\n");
                            send(i, resp, sizeof(resp), 0);
                            clients++;
                            memset(resp, 0, sizeof(resp));
                            strcpy(resp, "\n- - - - - - -Chat- - - - - - -");
                            send(i, resp, sizeof(resp), 0);

                            //------------------------------
                            // END ACCEPT FUNCTION
                            //------------------------------
                        }
                        if (clients == 2) //Start Game
                        {
                            memset(resp, 0, sizeof(resp));
                            strcpy(resp, "\n- - - - - - -End Chat- - - - - -\n- - - - - - -\
                            All Players Accounted for- - - - - - -\n\nPrepare Yourselves\n");

                            //------------------------------
                            // DEVELOP FUNCTION FOR MONSTER CREATION
                            //------------------------------

                            //1. Randomize Monster (Name, Attack, Health, Random Client to attack)
                            Monster M;
                            M.Name = "Your Boi";
                            M.HP = randrnge(500, 800);
                            M.PATK = randrnge(5, 10);
                            for(j = 0; j <= fdmax; j++) 
                            {
                                if (FD_ISSET(j, &master)) {
                                    if (j != listener) {
                                        send(j, resp, sizeof(resp), 0);
                                    }
                                }
                            }
                            memset(resp, 0, sizeof(resp));
                            strcpy(resp, "Monster name: Your Boi \n Attack range: 5-10\n Health range: 500-800");
                            for(j = 0; j <= fdmax; j++) 
                            {
                                if (FD_ISSET(j, &master)) {
                                    if (j != listener) {
                                        send(j, resp, sizeof(resp), 0);
                                    }
                                }
                            }
                            //------------------------------
                            // END FUNCTION FOR MONSTER CREATION
                            //------------------------------
                            strcpy(resp, "|||||||||||||||||| \n ||||||||||||||||||\n ||||||||||||||||||");
                            //------------------------------
                            // DEVELOP FUNCTION FOR CLIENT STATS (ATTACK / HP)
                            // Contents for each player already made the following 
                            // info will be attached once function created
                            //------------------------------
			               
                            Client Player1;
                            Player1.Name = "Boi 1";
                            Player1.HP = 100;
                            Player1.PATK = 8;
                            Player1.fd = i;
                            memset(resp, 0, sizeof(resp));
                            strcpy(resp, "Client name: Boi 1 \n Attack: 8\n Health : 100");
                            for(j = 0; j <= fdmax; j++) 
                            {
                                if (FD_ISSET(j, &master)) {
                                    if (j != listener) {
                                        send(j, resp, sizeof(resp), 0);
                                    }
                                }
                            }

                            //------------------------------
                            // END FUNCTION FOR CLIENT STATS (ATTACK / HP)
                            //------------------------------



                            //------------------------------
                            // DEVELOP FUNCTION FOR CHAT (figure out where to put)
                            //------------------------------

                            //TODO Add in game mechanics here
                            //2. Set up the Move Actions ([Attack], Guard, Heal)
                            //3. Send these options to all Clients
                            //4. Add a queue 
                        }
                            //Chat in Room
                            //for(j = 0; j <= fdmax; j++) 
                            //{ 
                            // send to everyone!
                                //if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                    //if (j != listener && j != i) {
                                    //    send(j, buf, nbytes, 0);
                                    //}
                               // }
                        //}
                    }
                    printf("finished, waiting\n");
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}
