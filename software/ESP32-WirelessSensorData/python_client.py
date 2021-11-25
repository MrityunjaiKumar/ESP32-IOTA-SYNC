import sys
import socket

if len(sys.argv) == 3:
    # Get "IP address of Server" and also the "port number" from argument 1 and argument 2
    ip = sys.argv[1]
    port = int(sys.argv[2])
else:
    print("Run like : python3 client.py <arg1 server ip 192.168.1.102> <arg2 server port 4444 >")
    exit(1)

# Create socket for server
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
s.settimeout(10);
print("Do Ctrl+c to exit the program !!")

# Let's send data through UDP protocol
while True:
    send_data = "Check";
    s.sendto(send_data.encode('utf-8'), (ip, port))
    data = s.recv(4096)
    print("\n\nClient received : ", data.decode('utf-8'), "\n\n")
# close the socket
s.close()