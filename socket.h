#ifndef SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <map>

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string>
#include <set>

#define LOCALHOST_PORT 8001

#define MAX_BUFFER_SIZE 4096

using namespace std;

class Socket {
    private:
        int port;
        int serverFd;
        string ip;
        //string name;

        struct sockaddr_in serv_addr;

        char buffer[MAX_BUFFER_SIZE];

        set<int> connections;

        void Error(const char * msg);
    public:
        int sockfd;
        static const int max_clients = 64;

        Socket(string ip, int port);

        pair<int, string> Accept();

        void Bind();

        void Broadcast(string message);

        void Connect(int hostPort);

        void Disconnect();

        void Disconnect(int clientfd);

        void Listen();

        string Read(int connfd, int bufferSize = MAX_BUFFER_SIZE);

        void Write(string message, int destSocketFd = -1);
};

#endif
