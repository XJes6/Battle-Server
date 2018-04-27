import socket 
import sys
import select
import time


host = '140.141.132.46' 
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
# inputs = [server, sys.stdin]
while inputs:
    
    print("DBG> Waiting for next available input", file=sys.stderr)
    
    # Blocking call to select() waiting for I/O channels to be "ready"
    
    readset, writeset, exceptset = select.select(inputs, [], [])
    print("DBG> select() returned, so there must be some work to do", file=sys.stderr)
    
    #iterate over items in the readset
    for channel in readset:
        if channel is server:
            # client is making a connect(), so accept() would not block
            
            peer, addr = server.accept()
            print('Accepted connection from', addr, file=sys.stderr)
            inputs.append(peer)
        else:
            data = channel.recv(recvsize)
            if data != b"":
                print("{}: {}".format(channel.getpeername(), data), file=sys.stderr)
                if data == b"Accept": #Exit Room State
                    if peer not in accepted:
                        accepted.append(peer)
                        clients = clients + 1
                        peer.sendall(bytes("Acknowledged\t", "utf-8"))
                    for peers in accepted:
                        numplayers = str(clients) + " players has joined the game. Please wait for " \
                                            + str(MAXPLAYERS - clients) + " more to join...\t"
                        peers.sendall(bytes(numplayers, "utf-8"))
                    #time.sleep(.5)
                if clients == 1:
                    for peers in accepted:
                        peers.sendall(bytes("Prepare Yourselves \t", "utf-8"))
                    print("sent")
                    
            else:
                print('DBG> Closing', channel.getpeername(), file=sys.stderr)
                inputs.remove(channel)
                channel.close()
                if len(inputs) == 1:
                    inputs.pop()
                
print("DBG> No more available channels, terminating loop", file=sys.stderr)
server.close()

