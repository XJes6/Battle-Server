import socket 
import sys

host = '140.141.225.187' 
port = 50000 
size = 2048
s = socket.socket(socket.AF_INET, 
                  socket.SOCK_STREAM)

server = (host, port)
print('ActiveHost0 about to connect to:', server)
s.connect(server)
if s == None:
    sys.exit()
inroom = input("When you are ready to proceed type 'Accept' and hit enter: ")
while inroom != "Accept":
    s.sendall(inroom.encode())
    inroom = input("Please type 'Accept' to proceed: ")
if inroom == "Accept":
    s.sendall(inroom.encode())
recv = s.recv(size)
print(recv)
userline = input("Enter line to send (empty line to terminate): ")
while userline != "":
    s.sendall(userline.encode())
    userline = input("Enter line to send (empty line to terminate): ")
print("Finished sending")

s.close()
