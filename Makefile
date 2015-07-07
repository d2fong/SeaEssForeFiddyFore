
client: Client.cpp rpc_client.cpp client1.c
	g++ Client.cpp rpc_client.cpp client1.c -o client

binder: Binder.cpp binder_main.cpp
	g++ Binder.cpp binder_main.cpp -o binder

client_class: Client.cpp
	g++ Client.cpp -o client_class
student_binder_vars: set_binder_vars.cpp
	g++ set_binder_vars.cpp -o set_vars
