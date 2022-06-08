#include "socket.h"

Socket::Socket(string ip, int port) {
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    this->port = port;
	this->ip = ip;

    if (this->sockfd < 0) 
        Error("ERROR opening socket");

    /* clear address structure */
    explicit_bzero((char *) &serv_addr, sizeof(serv_addr));
}

void Socket::Error(const char * msg) {
	perror(msg);	
	exit(1);
}

pair<int,string> Socket::Accept() {

	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);

	int newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &cli_len);

	if (newsockfd == -1) {
		Error("Server didn't accept\n");
	}

	connections.insert(newsockfd);
	
	return pair<int, string>(newsockfd, inet_ntoa(cli_addr.sin_addr));
}

void Socket::Bind(){
	/* IPV4 */
	this->serv_addr.sin_family = AF_INET;

	/* Binds the socket to all available interfaces */
	this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Port converted into network byte order */
	this->serv_addr.sin_port = htons(this->port);

	int status = bind(this->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (status == -1){
		Error("Could not bind with the server\n");
	}
}

void Socket::Broadcast(string message) {
	for(int connectedSockedFd: connections) {
		Write(message, connectedSockedFd);
	}
}

void Socket::Connect(int hostPort) {

	serv_addr.sin_family = AF_INET;
	
	serv_addr.sin_addr.s_addr = inet_addr((this->ip).c_str());

	serv_addr.sin_port = htons(hostPort);

	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		Error("Error while connecting\n");
}

void Socket::Listen() {
	listen(this->sockfd, max_clients);
}

void Socket::Disconnect() {
	int status = close(this->sockfd);

	if (status == -1) {
		Error("Could not disconnect... =(\n");
	}
}

void Socket::Disconnect(int clientfd) {
	int status = close(clientfd);
	if (status == -1) {
		Error("Could not disconnect... =(\n");
	}

	connections.erase(clientfd);
}

string Socket::Read(int connfd) {
	if(read(connfd, buffer, MAX_BUFFER_SIZE) == -1)
		Error("Error on reading message");

	string response(buffer);

	explicit_bzero(buffer, MAX_BUFFER_SIZE);
	return response;
}

void Socket::Write(string message, int destSockedFd) {
	if(destSockedFd == -1)
		destSockedFd = sockfd;

	write(destSockedFd, message.c_str(), message.size());
}