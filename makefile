client:
	g++ socket.cpp client.cpp -o client

server:
	g++ socket.cpp server.cpp -o server -pthread
	