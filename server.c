#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT 8888 //port
#define TRUE 1
#define FALSE 0

int main(int argc, char const *argv[])
{
    fd_set master;
    fd_set read_fds;
    int fdmax, listener, newfd, nbytes;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char buf[1024]; //buffer for client data
    char *hello = "Test from server";

    FD_ZERO (&master);
    FD_ZERO (&read_fds);
    //FD_ZERO (&write_fds);

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //listen
    if(listen(listener, 10) == -1)
    {
        perror("listen error");
        exit(3);
    }
    printf("listening\n");
    //add the listener to master set
    FD_SET(listener, &master);

    //biggest fd
    fdmax = listener; //initialize
    while(1) 
    {
        printf("Waiting\n");
        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
        {
                perror("select error");
                exit(4);
        }
        for(int conn = 0; conn <=fdmax; conn++)
        {
            if(FD_ISSET(conn, &read_fds))
            {
                if(conn == listener)
                {
                    //new connections
                    addrlen = sizeof(&address);
                    newfd = accept(listener, (struct sockaddr*) &address, &addrlen);
                    // add accept error
                    FD_SET(newfd, &master); //add to master set
                    if (newfd > fdmax)
                    {fdmax = newfd;}
                    fprintf(stderr, "New Connection (socket)");
                }
            }
            else
            {
                if ((nbytes = recv(conn, buf, sizeof buf, 0)) <= 0)
                {
                    if(nbytes == 0) {fprintf(stderr, "Connection closed (socket)");}
                    close(1);
                    FD_CLR(conn, &master); //remove from master set
                }
                else
                {
                    for(int i = 0; i <= fdmax; i++)
                    {
                        //send to everyone
                        if (FD_ISSET(i, &master))
                        {
                            //except listener and ourselves
                            if(i != listener && i != conn)
                            {
                                send(i, buf, nbytes, 0);
                            }
                        }
                    }
                }
            }

        }
    }
    return 0;
}