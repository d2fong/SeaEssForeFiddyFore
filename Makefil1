client: Client.cpp rpc_client.cpp client1.c helpers.cpp
		g++ -std=c++0x Client.cpp rpc_client.cpp client1.c helpers.cpp DB.cpp -o client

binder: Binder.cpp binder_main.cpp
		g++ -std=c++0x Binder.cpp binder_main.cpp helpers.cpp DB.cpp -o binder

client_class: Client.cpp
		g++ Client.cpp -o client_class
student_binder_vars: set_binder_vars.cpp
		g++ set_binder_vars.cpp -o set_vars

server: sample_code/server.c sample_code/server_functions.c sample_code/server_function_skels.c rpc_server.cpp helpers.cpp Server.cpp 
		g++ -std=c++0x sample_code/server.c sample_code/server_functions.c sample_code/server_function_skels.c Message.cpp Server.cpp DB.cpp helpers.cpp rpc_server.cpp -o server

server1: sample_code/server1.c sample_code/server_functions.c sample_code/server_function_skels.c rpc_server.cpp helpers.cpp Server.cpp 
		g++ -std=c++0x sample_code/server1.c sample_code/server_functions.c sample_code/server_function_skels.c Message.cpp Server.cpp DB.cpp helpers.cpp rpc_server.cpp -o server1
