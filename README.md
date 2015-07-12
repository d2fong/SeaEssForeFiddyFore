CS 454 A2:

Team Members:
	Name: Shayan Masood
	id: szmasood
	student #: 20430249

	Dylan Fong
	id: d2fong
	student #: 20433243


How to compile:

	To compile the code, you need to run make:

			make default

	This will create the binder executable named 'binder' and the librpc.a library to be used


Using the library:
	
	To use the library with your own servers and client

	You must compile your server/client like so:


			g++ -L {client.o} -lrpc -o {client.o} -o client

			g++ -L {server_functions.o server_function_skels.o server.o} -lrpc -o server


	Then, you must initialize the binder like so:


			./binder

	Manually set the binder addrses and port on the server/client machines


	Run the server and client

			./server
			./client

Dependencies:

	There are no dependencies, but the c++ version must be c++11.



