client:
	g++ socket.cpp client.cpp -o client -pthread

server:
	g++ socket.cpp server.cpp -o server -pthread
	
