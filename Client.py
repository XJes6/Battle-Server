import socket
import sys
import select

def msglen(msg):
    count = 0
    for i in msg:
        count = count + 1
    return count
host = '127.0.0.1'
port = 9034
size = 2048
s = socket.socket(socket.AF_INET,
                  socket.SOCK_STREAM)

AcceptFlag = 0


server = (host, port)
print('ActiveHost0 about to connect to:', server)
s.connect(server)
inputs = [ server ]

if s == None: #if the server is not connected: Exit
    sys.exit()
print("Welcome to Battle-Server")
print("When you are ready to proceed type 'Accept' and hit enter")
while True:
    readset, writeset, exceptset = select.select([sys.stdin, s], [], [])
    for read in readset:
        if read is s:
            recv = (s.recv(size))
            recv = (recv.decode("utf-8"))
            print(recv[0:-1])

        else:
            #--------------------------------------------------------
            #Room State
            #--------------------------------------------------------
            inroom = sys.stdin.readline()
            while inroom != "Accept\n" and AcceptFlag != 1:
                print("Please type 'Accept' to proceed: ")
                inroom = sys.stdin.readline()
            AcceptFlag = 1
            #if ((inroom == "Attack\n") | (inroom == "Defend\n") | (inroom == "Heal\n")):
            #    s.sendall(inroom.encode())
            #    recv = (s.recv(size))
            #    recv = (recv.decode("utf-8"))
            #    print(recv[0:-1])
            s.sendall(inroom.encode())


#inroom = input("When you are ready to proceed type 'Accept' and hit enter: ")
#while inroom != "Accept":
#    s.sendall(inroom.encode())
#    inroom = input("Please type 'Accept' to proceed: ")
#if inroom == "Accept":
#    s.sendall(inroom.encode())
#recv = str(s.recv(size), "utf-8")
#while recv != "":
#    msg = ""
#    for i in recv:
#        msg = msg + i
#        if i == "\t":
#            break
#    size = msglen(msg)
#    print(msg)
#    recv = recv[size: -1]
#    if msg == "Prepare Yourselves":
#        print("here")


#while msg != "Prepare Yourselves":
#    print(msg)
#    msg = ""
#    recv = recv[size: -1]
#    for i in recv:
#        msg = msg + i
#        if i == "\t":
#            break
#    print(msg)
#    size = msglen(msg)
#print(msg)
# - - - - - - - - - - - - -
# Potential Chat Box State
# - - - - - - - - - - - - -


userline = input("Enter line to send (empty line to terminate): ")
while userline != "":
    s.sendall(userline.encode())
    userline = input("Enter line to send (empty line to terminate): ")
print("Finished sending")

s.close()
