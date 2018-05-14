/*
** server.c -- a battle server game with chat mechanics
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "Queue.h"
#include "ctcp_linked_list.c"

#define PORT "9034"   // port we're listening on
#define STDIN 0 //STDIN
#define MAXDATASIZE 256

/*This is a struct for a Monster. It has 3 parameters
  A Name, Health, and Physical Attack */

typedef struct Monster
{
    char* Name;
    int HP;
    int PATK;

}Monster;

/*This is a struct for a Player. It has 5 parameters
  A Name, Heath, Physical Attack, File Descriptor, and Defense*/

typedef struct Client
{
    char* Name;
    int HP;
    int PATK;
    int fd;
    int DEF;

}Client;

/*This function's purpose is to delay is to slow down
  how fast the cpu sends msgs to the Clients
  This function takes in one parameter, int */

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
 
    // Stroing start time
    clock_t start_time = clock();
 
    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds)
        ;
}

/*This function's purpose is to generate the monster's stats */
Monster MonsterGen(Monster M, int i, int j, int fdmax, int listener, fd_set master)
{
    char resp[MAXDATASIZE];
    M.Name = "Slime Boss";
    M.HP = 500;
    M.PATK = 30;
    memset(resp, 0, sizeof(resp));
    strcpy(resp, "\nMonster name: Slime Boss \nAttack range: 5-10\nHealth range: 500-800\n");
    for(j = 0; j <= fdmax; j++)
    {
        if (FD_ISSET(j, &master)) {
            if (j != listener) {
                send(j, resp, sizeof(resp), 0);
                delay(3);
            }
        }
    }
  return M;
}
/*This functions purpose is to send a message to the client when they accept the game*/
void acceptance(int i)
{
    char resp[MAXDATASIZE];
    memset(resp, 0, sizeof(resp));
    strcpy(resp, "\n- - - - - - -\nAcknowledged |\n- - - - - - -\n\n- - - - - - -Chat Active- - - - - - -\n");
    send(i, resp, sizeof(resp), 0);
}

/*This function's purpose is to generate the Player inital stats */
Client PlayerGen(Client P, int i, int j, int fdmax, int listener, fd_set master)
{
    char resp[MAXDATASIZE];
    P.HP = 100;
    P.PATK = 30;
    P.DEF = 10;
    P.fd = i;
    memset(resp, 0, sizeof(resp));
    strcpy(resp, "Player Stats:\nClient name: The Hero \nAttack: 30\nHealth : 100\n - - - - - - - - \n");
    for(j = 0; j <= fdmax; j++)
    {
        if (FD_ISSET(j, &master)) {
            if (j != listener) {
                send(j, resp, sizeof(resp), 0);
                delay(3);
            }
        }
    }
    return P;
}
/*This function's purpose is to help the system change the Player's stats after their choices */
Client Player_stats(Queue *history, Client Player, Monster M)
{
        //system plays
        if (strcmp(front(history), "Defend")== 0){Player.HP = (Player.HP + (8));}
        if (strcmp(front(history), "Heal") == 0)
        {
            if (Player.HP + (Player.HP*0.33) >= Player.HP){Player.HP = 100;}
            else {Player.HP = (Player.HP*0.33);}

        }
        return Player;
}
/*This function's purpose is to check if the Monster Wins or if the Clients with */
void End_Game(Monster M, Client Player, int j, int fdmax, int listener, fd_set master, int clients, linked_list_t *fdlist)
{
  char resp[MAXDATASIZE];
  if(M.HP <= 0)
  {
      memset(resp, 0, sizeof(resp));
      strcpy(resp, "You Win.");
      for(j = 0; j <= fdmax; j++)
      {
          if (FD_ISSET(j, &master)) {
              if (j != listener) {
                send(j, resp, sizeof(resp), 0);
                close(j); // bye!
                FD_CLR(j, &master); // remove from master set
                ll_node_t *node = fdlist->head;
                ll_remove(fdlist, node);
                clients--;
              }
          }
      }
  }
  if(Player.HP <= 0)
  {
      memset(resp, 0, sizeof(resp));
      strcpy(resp, "You Lose.\nBetter luck next time.");
      for(j = 0; j <= fdmax; j++)
      {
          if (FD_ISSET(j, &master)) {
              if (j != listener) {
                send(j, resp, sizeof(resp), 0);
                close(j); // bye!
                FD_CLR(j, &master); // remove from master set
                ll_node_t *node = fdlist->head;
                ll_remove(fdlist, node);
                clients--;
              }
          }
      }
  }
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{

		return &(((struct sockaddr_in*)sa)->sin_addr);

}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    Queue *history = createQueue(4); //Queue of Capacity 4
    //int turn_count = 0;
    Client Player;  //Player Creation
    Monster M;      //Monster Creation
    linked_list_t *fdlist;
    linked_list_t *Players;
    fdlist = ll_create();
    Players = ll_create();

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[MAXDATASIZE];    // buffer for client data
    char msg[MAXDATASIZE];    // buffer for STDIN
    char resp[MAXDATASIZE];   // buffer for responses
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
    fprintf(stderr, "Server Has Started\n");
    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
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
                        fprintf(stderr, "selectserver: new connection from %s on "
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
                    char c[MAXDATASIZE];
                    memset(c, 0, sizeof(c));
                    sprintf(c, "SERVER: %s", msg);
                    if(strcmp(msg, "Stop\n")== 0)
                    {
                        strcpy(c, "Server is now closing");
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, c, sizeof(c), 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                        close(i);
                    }
                    else
                    {
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, c, msgbytes+10, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                }

                else
                {

                    memset(buf, 0, sizeof(buf));
                    nbytes = recv(i, buf, sizeof(buf), 0);
                    // handle data from a client
                    if (nbytes <= 1) { //CONNECTION KILLED
                        // got error or connection closed by client
                        if (nbytes <= 1)
                        {
                            // connecton closed
                            fprintf(stderr, "selectserver: socket %d hung up\n", i);
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                        ll_node_t *node = fdlist->head;
                        ll_remove(fdlist, node);
                        clients--;
                    } else {
                        int inside = 2;
                        inside = ll_find(fdlist, i);
                        // we got some data from a client
                        if (strcmp(buf, "Accept\n") == 0 && inside == 0)
                        {
                            ll_add(fdlist, &i, i);
                            clients++;
                            acceptance(i);

                            if (clients == 4) //Start Game
                            {
                              memset(resp, 0, sizeof(resp));
                              strcpy(resp, "\n- - - - - - -All Players Ready- - - - - - -\n\nPrepare Yourselves\n- - - - - - - \n");
                              for(j = 0; j <= fdmax; j++)
                              {
                                  if (FD_ISSET(j, &master)) {
                                      if (j != listener) {
                                          send(j, resp, sizeof(resp), 0);
                                      }
                                  }
                              }
                              M = MonsterGen(M, i, j, fdmax, listener, master); //sends Monster Stats
  			                  Player = PlayerGen(Player, i, j, fdmax, listener, master); //sends Player Stats
                            }
                        }
                        else if (strcmp(buf, "Attack\n") == 0)
                        {
                            Enqueue(history, buf);
                            memset(resp, 0, sizeof(resp));
                            sprintf(resp, "Client %i has chosen to ATTACK\n", i);
                            for(j = 0; j <= fdmax; j++)
                            {
                                if (FD_ISSET(j, &master)) {
                                    if (j != listener && j != i) {
                                        send(j, resp, sizeof(resp), 0);
                                    }
                                }
                            }
                            if (Qlength(history) == 4)
                            {
                                for(int o = 0; o = Qlength(history); o++)
                                {
                                    if (strcmp(front(history), "Attack") == 0) {M.HP = M.HP - Player.PATK;}
                                    Player = Player_stats(history, Player, M);
                                    history = Dequeue(history);
                                }
                                Player.HP = Player.HP - M.PATK;
                                //SEND Update TO PLAYERS
                                memset(resp, 0, sizeof(resp));
                                sprintf(resp, "\n- - - - - UPDATE - - - - - \nMonster name: Slime Boss \nMonster has %i of HP left\nYou have %i of HP left\n - - - - - - - - - - - - - - \n", M.HP, Player.HP);
                                for(j = 0; j <= fdmax; j++)
                                {
                                    if (FD_ISSET(j, &master)) {
                                        if (j != listener) {
                                            send(j, resp, sizeof(resp), 0);
                                            delay(3);
                                        }
                                    }
                                }
                                End_Game(M, Player, j, fdmax, listener, master, clients, fdlist);
                            }
                        }
                        else if (strcmp(buf, "Defend\n") == 0)
                        {
                            Enqueue(history, buf);
                            memset(resp, 0, sizeof(resp));
                            sprintf(resp, "Client %i has chosen to DEFEND\n", i);
                            for(j = 0; j <= fdmax; j++)
                            {
                                if (FD_ISSET(j, &master)) {
                                    if (j != listener && j != i) {
                                        send(j, resp, sizeof(resp), 0);
                                    }
                                }
                            }
                            if (Qlength(history) == 4)
                            {
                                for(int o = 0; o = Qlength(history); o++)
                                {
                                    if (strcmp(front(history), "Attack") == 0) {M.HP = M.HP - Player.PATK;}
                                    Player = Player_stats(history, Player, M);
                                    history = Dequeue(history);\
                                }
                                Player.HP = Player.HP - M.PATK;
                                //SEND Update TO PLAYERS
                                memset(resp, 0, sizeof(resp));
                                sprintf(resp, "\n- - - - - UPDATE - - - - - \nMonster name: Slime Boss \nMonster has %i of HP left\nYou have %i of HP left\n - - - - - - - - - - - - - - \n", M.HP, Player.HP);
                                for(j = 0; j <= fdmax; j++)
                                {
                                    if (FD_ISSET(j, &master)) {
                                        if (j != listener) {
                                            send(j, resp, sizeof(resp), 0);
                                            delay(3);
                                        }
                                    }
                                }

                              End_Game(M, Player, j, fdmax, listener, master, clients, fdlist);
                            }
                        }
                        else if (strcmp(buf, "Heal\n") == 0)
                        {
                            Enqueue(history, buf);
                            memset(resp, 0, sizeof(resp));
                            sprintf(resp, "Client %i has chosen to HEAL\n", i);
                            for(j = 0; j <= fdmax; j++)
                            {
                                if (FD_ISSET(j, &master)) {
                                    if (j != listener && j != i) {
                                        send(j, resp, sizeof(resp), 0);
                                    }
                                }
                            }
                            if (Qlength(history) == 4)
                            {
                                for(int o = 0; o = Qlength(history); o++)
                                {
                                    if (strcmp(front(history), "Attack") == 0) {M.HP = M.HP - Player.PATK;}
                                    Player = Player_stats(history, Player, M);
                                    history = Dequeue(history);
                                }
                                Player.HP = Player.HP - M.PATK;
                                //SEND Update TO PLAYERS
                                memset(resp, 0, sizeof(resp));
                                sprintf(resp, "\n- - - - - UPDATE - - - - - \nMonster name: Slime Boss\nMonster has %i of HP left\nYou have %i of HP left\n - - - - - - - - - - - - - \n", M.HP, Player.HP);
                                for(j = 0; j <= fdmax; j++)
                                {
                                    if (FD_ISSET(j, &master)) {
                                        if (j != listener) {
                                            send(j, resp, sizeof(resp), 0);
                                            delay(3);
                                        }
                                    }
                                }

                                End_Game(M, Player, j, fdmax, listener, master,  clients, fdlist);
                            }
                        }
                        else
                        {
                            char c[MAXDATASIZE];
                            memset(c, 0, sizeof(c));
                            sprintf(c, "Chat: %s", buf);
                            c[nbytes+5] = '\0';
                            //Chat in Room
                            for(j = 0; j <= fdmax; j++)
                            {
                            // send to everyone!
                                if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                    int inside = 2;
                                    inside = ll_find(fdlist, j);
                                    if (j != listener && j != i && inside == 1) {
                                        send(j, c, nbytes+6, 0);
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
