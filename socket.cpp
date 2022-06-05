#include "socket.h"

Socket::Socket(int port) {

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    this->port = port;

    if (this->sockfd < 0) 
        Error("ERROR opening socket");

    /* clear address structure */
    explicit_bzero((char *) &serv_addr, sizeof(serv_addr));
}

void Socket::Error(const char * msg) {
	perror(msg);
	exit(1);
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
		return;
	}
}

void Socket::Listen() {
	listen(this->sockfd, max_clients);
}

pair<int,string> Socket::Accept() {

	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);

	int newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &cli_len);

	if (newsockfd == -1) {
		Error("Server didn't accept\n");
	}
	
	return pair<int, string>(newsockfd, inet_ntoa(cli_addr.sin_addr));
}