import socket
import sys
import select
import time


host = '140.141.132.47'
port = 5011

backlog = 5
recvsize = 1024

MAXPLAYERS = 4
# server's listener socket
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Release listener socket immediately when program exits,
# avoid socket.error: [Errno 48] Address already in use
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server.bind((host,port))

print('PassiveHost0 listening on port', port)
server.listen(backlog)

inputs = [ server ]        # Maintain the list of all sockets/fds from which we may get input
clients = 0
accepted = []
inputs.append(sys.stdin)
# inputs = [server, sys.stdin]
while inputs:

    #print("DBG> Waiting for next available input", file=sys.stderr)

    # Blocking call to select() waiting for I/O channels to be "ready"

    readset, writeset, exceptset = select.select(inputs, accepted, [])
    #print("DBG> select() returned, so there must be some work to do", file=sys.stderr)

    #iterate over items in the readset
    for channel in readset:
        if channel is server:
            # client is making a connect(), so accept() would not block

            peer, addr = server.accept()
            print('Accepted connection from', addr, file=sys.stderr)
            inputs.append(peer)

        elif channel is sys.stdin:
            inroom = sys.stdin.readline()
        else:
            data = channel.recv(recvsize)
            if data != b"":
                print("{}: {}".format(channel.getpeername(), data), file=sys.stderr)
                if data == b'Accept\n': #Exit Room State
                    client = channel.getpeername()
                    
                    #TODO we need to be able to distinguish between clients using the same socket
                    

                    print("- - - - Client- - - - ")
                    print(client)
                    print(" - - - - - - - \n")
                    
                    if peer not in accepted:
                        peer.sendall(bytes("\n- - - - - - -\nAcknowledged |\n- - - - - - -\n", "utf-8"))
                        clients = clients + 1
                        accepted.append(peer)
                    #accepted2 = list(accepted.keys())
                    print("- - - - - - - - -\n ")
                    numplayers = str(clients) + " players has joined the game. Please wait for " \
                                       + str(MAXPLAYERS - clients) + " more to join...\n"                    
                    for o in accepted:
                        o.sendall(bytes(numplayers, "utf-8"))

                #Chat?
                #for peers in AClients:
                    #if peer != peers and data != b'Accept\n':
                        #peers.sendall(data)

                #if clients == 3:
                    #for peers in range(len(accepted2)):
                    #    accepted[accepted2[peers]].sendto(bytes(numplayers, "utf-8"), accepted2[peers])
                    #print("sent")

            else:
                print('DBG> Closing', channel.getpeername(), file=sys.stderr)
                inputs.remove(channel)
                channel.close()
                if len(inputs) == 1:
                    inputs.pop()

print("DBG> No more available channels, terminating loop", file=sys.stderr)
server.close()
