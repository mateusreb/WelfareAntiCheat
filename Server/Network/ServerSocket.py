import socket

class Server:
    udp = 0
    def Start(self, port):        
        self.udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        orig = ("", port)
        self.udp.bind(orig)

    def Shutdown(self):
        self.udp.close()

    def Read(self):        
        while True:
            data, client = self.udp.recvfrom(1024)
            self.udp.sendto(data, client)
            print("[" + client[0] + ":" + str(client[1]) + "] " + data.decode('utf-8'))
