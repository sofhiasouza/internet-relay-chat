#ifndef SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string>

#define LOCALHOST 8001

using namespace std;

class Socket {
    private:
        int port;
        struct sockaddr_in serv_addr;

        void Error(const char * msg);

    public:
        int sockfd;
        static const int max_clients = 64;

        Socket(int port);

        pair<int, string> Accept();

        void Bind();

        void Listen();
};

#endif