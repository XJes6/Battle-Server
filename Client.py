import socket 
import sys

def msglen(msg):
    count = 0
    for i in msg:
        count = count + 1
    return count
host = '140.141.132.46' 
port = 5011
size = 2048
s = socket.socket(socket.AF_INET, 
                  socket.SOCK_STREAM)

server = (host, port)
print('ActiveHost0 about to connect to:', server)
s.connect(server)
inputs = [ server ]

if s == None: #if the server is not connected: Exit
    sys.exit()
#--------------------------------------------------------
#Room State
#--------------------------------------------------------    
inroom = input("When you are ready to proceed type 'Accept' and hit enter: ")
while inroom != "Accept":
    s.sendall(inroom.encode())
    inroom = input("Please type 'Accept' to proceed: ")
if inroom == "Accept":
    s.sendall(inroom.encode())
recv = str(s.recv(size), "utf-8")
while recv != "":
    msg = ""
    for i in recv:
        msg = msg + i
        if i == "\t":
            break
    size = msglen(msg)
    print(msg)
    recv = recv[size: -1]
    if msg == "Prepare Yourselves":
        print("here")


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
