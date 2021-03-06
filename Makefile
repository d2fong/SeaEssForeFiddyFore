.PHONY: clean all



default: binder librpc.a

librpc.a: Client.o Db.o helpers.o rpc_client.o rpc_server.o Server.o
	ar rcs librpc.a Client.o Db.o helpers.o rpc_client.o rpc_server.o Server.o

binder: Binder.o binder_main.o helpers.o DB.o
	g++ Binder.o binder_main.o helpers.o DB.o -o binder


client: Client.o rpc_client.o client1.o helpers.o DB.o
	g++ Client.o rpc_client.o client1.o helpers.o DB.o -o client

server: server.o server_functions.o server_function_skels.o rpc_server.o helpers.o Server.o 
	g++ server.o server_functions.o server_function_skels.o Server.o DB.o helpers.o rpc_server.o -o server

client_link_test: client1.o librpc.a
	g++ -L . client1.o -lrpc -o client1

server_link_test: server.o librpc.a
	g++ -L . server.o -lrpc -o server1

# convenience method
all: binder client server

clean:
	rm *.o

# object rules
Binder.o: Binder.cpp
	g++ -c Binder.cpp

binder_main.o: binder_main.cpp
	g++ -c binder_main.cpp

client1.o: client1.c
	g++ -c client1.c

Client.o: Client.cpp
	g++ -c Client.cpp

DB.o: DB.cpp
	g++ -c DB.cpp

helpers.o: helpers.cpp
	g++ -c helpers.cpp

rpc_client.o: rpc_client.cpp
	g++ -c rpc_client.cpp

rpc_server.o: rpc_server.cpp
	g++ -c rpc_server.cpp

server.o: sample_code/server.c sample_code/server_functions.c sample_code/server_function_skels.c
	g++ -c sample_code/server.c sample_code/server_functions.c sample_code/server_function_skels.c

server_functions.o: sample_code/server_functions.c
	g++ -c sample_code/server_functions.c

server_function_skels.o: sample_code/server_function_skels.c
	g++ -c sample_code/server_function_skels.c

Server.o: Server.cpp
	g++ -c Server.cpp
