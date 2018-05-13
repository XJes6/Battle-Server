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
#include "ctcp_linked_list.c"

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
    int DEF;

}Client;

Monster MonsterGen(Monster M, int i, int j, int fdmax, int listener, fd_set master)
{
    char resp[1024];
    M.Name = "Your Boi";
    M.HP = 800;
    M.PATK = 30;
    memset(resp, 0, sizeof(resp));
    strcpy(resp, "\nMonster name: Your Boi \nAttack range: 5-10\nHealth range: 500-800\n");
    for(j = 0; j <= fdmax; j++)
    {
        if (FD_ISSET(j, &master)) {
            if (j != listener) {
                send(j, resp, sizeof(resp), 0);
            }
        }
    }
  return M;
}

void acceptance(int i)
{
    char resp[1024];
    memset(resp, 0, sizeof(resp));
    strcpy(resp, "\n- - - - - - -\nAcknowledged |\n- - - - - - -\n");
    send(i, resp, sizeof(resp), 0);
    memset(resp, 0, sizeof(resp));
    strcpy(resp, "\n- - - - - - -Chat Active- - - - - - -");
    send(i, resp, sizeof(resp), 0);
}

Client PlayerGen(Client P, int i, int j, int fdmax, int listener, fd_set master)
{
    //------------------------------
    // DEVELOP FUNCTION FOR CLIENT STATS (ATTACK / HP)
    // Contents for each player already made the following
    // info will be attached once function created
    //------------------------------
    char resp[1024];
    P.HP = 100;
    P.PATK = 10;
    P.DEF = 10;
    P.fd = i;
    memset(resp, 0, sizeof(resp));
    strcpy(resp, "Player Stats:\nClient name: The Hero \nAttack: 8\nHealth : 100\n - - - - - - - - ");
    for(j = 0; j <= fdmax; j++)
    {
        if (FD_ISSET(j, &master)) {
            if (j != listener) {
                send(j, resp, sizeof(resp), 0);
            }
        }
    }
    return P;
}

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

void End_Game(Monster M, Client Player, int j, int fdmax, int listener, fd_set master)
{
  char resp[1024];
  if(M.HP <= 0)
  {
      memset(resp, 0, sizeof(resp));
      strcpy(resp, "You Win.");
      for(j = 0; j <= fdmax; j++)
      {
          if (FD_ISSET(j, &master)) {
              if (j != listener) {
                  send(j, resp, sizeof(resp), 0);
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
    Queue *history = createQueue(4);
    int turn_count = 0;
    Client Player;
    Monster M;
    linked_list_t *fdlist;
    linked_list_t *Players;
    fdlist = ll_create();
    Players = ll_create();

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
    fprintf(stderr, "Server Has Started\n");
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
                    char c[1024];
                    memset(c, 0, sizeof(c));
                    sprintf(c, "SERVER: %s", msg);
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
                              strcpy(resp, "\n- - - - - - -All Players Ready- - - - - - -\n\nPrepare Yourselves\n- - - - - - - ");
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
                            sprintf(resp, "Client %i has chosen to ATTACK", i);
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
                                    printf("size: %i\n", Qlength(history));
                                }
                                Player.HP = Player.HP - M.PATK;
                                //SEND Update TO PLAYERS
                                memset(resp, 0, sizeof(resp));
                                sprintf(resp, "\n- - - - - UPDATE - - - - - \nMonster name: Slime Boss \nMonster has %i of HP left\nYou have %i of HP left\n - - - - - - - - - - - - - - ", M.HP, Player.HP);
                                for(j = 0; j <= fdmax; j++)
                                {
                                    if (FD_ISSET(j, &master)) {
                                        if (j != listener) {
                                            send(j, resp, sizeof(resp), 0);
                                        }
                                    }
                                }
                                End_Game(M, Player, j, fdmax, listener, master);
                            }
                        }
                        else if (strcmp(buf, "Defend\n") == 0)
                        {
                            Enqueue(history, buf);
                            memset(resp, 0, sizeof(resp));
                            sprintf(resp, "Client %i has chosen to DEFEND", i);
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
                                sprintf(resp, "\n- - - - - UPDATE - - - - - \nMonster name: Slime Boss \nMonster has %i of HP left\nYou have %i of HP left\n - - - - - - - - - - - - - - ", M.HP, Player.HP);
                                for(j = 0; j <= fdmax; j++)
                                {
                                    if (FD_ISSET(j, &master)) {
                                        if (j != listener) {
                                            send(j, resp, sizeof(resp), 0);
                                        }
                                    }
                                }

                              End_Game(M, Player, j, fdmax, listener, master);
                            }
                        }
                        else if (strcmp(buf, "Heal\n") == 0)
                        {
                            Enqueue(history, buf);
                            memset(resp, 0, sizeof(resp));
                            sprintf(resp, "Client %i has chosen to HEAL", i);
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
                                sprintf(resp, "\n- - - - - UPDATE - - - - - \nMonster name: Slime Boss\nMonster has %i of HP left\nYou have %i of HP left\n - - - - - - - - - - - - - - ", M.HP, Player.HP);
                                for(j = 0; j <= fdmax; j++)
                                {
                                    if (FD_ISSET(j, &master)) {
                                        if (j != listener) {
                                            send(j, resp, sizeof(resp), 0);
                                        }
                                    }
                                }

                                End_Game(M, Player, j, fdmax, listener, master);
                            }
                        }
                        else
                        {
                            char c[1024];
                            memset(c, 0, sizeof(c));
                            sprintf(c, "Chat: %s", buf);
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
                    fprintf(stderr, "finished, waiting\n");
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
