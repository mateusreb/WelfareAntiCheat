#from MySQLdb import _mysql
from Network import ServerSocket

print("========================================================")
print("##################[SERVER-VER 0.0.0.1]##################")
print("========================================================")
print("[<] Starting the server.")
print("[>] Server running on port: 27015.")
print("--------------------------------------------------------")
def main():
    #db=_mysql.connect("localhost","root","","panel")
    Server = ServerSocket.Server();
    Server.Start(27015)
    Server.Read()

def testList():
    list = []
    while  len(list) < 10:
        list.append(1)

    print(list)

if __name__ == "__main__":
    #main()
    testList()
    