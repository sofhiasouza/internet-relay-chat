#ifndef SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet.h> 

using namespace std;

class Socket {

private:
    int port;
    struct sockaddr_in serv_addr;

public:
    int sockfd;
    static const int max_clients = 64;

    Socket(int port);

    void Bind();

    void Listen();

}


#endif