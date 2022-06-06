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
#include <vector>

#define LOCALHOST_PORT 8001

#define MAX_BUFFER_SIZE 4097

using namespace std;

class Socket {
    private:
        int port;
        int serverFd;

        struct sockaddr_in serv_addr;

        char buffer[MAX_BUFFER_SIZE];

        vector<int> connections;

        void Error(const char * msg);
    public:
        int sockfd;
        static const int max_clients = 64;

        Socket(int port);

        pair<int, string> Accept();

        void Bind();

        void Broadcast(string message);

        void Connect(char * hostname, int hostPort);

        void Disconnect();

        void Listen();

        string Read();

        void Write(string message, int destSocketFd = -1);
};

#endif