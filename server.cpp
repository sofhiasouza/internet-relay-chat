#include "socket.h"

#include <iostream>
#include <tuple>

#define SERVER_PORT 8192

using namespace std;

int main(int argc, char* argv[]){

    Socket* server = new Socket(SERVER_PORT);
    server->Bind();

    cout << "Server Connected :)";

    server->Listen();

    string ip;
    int client;
    tie(client, ip) = server->Accept();

    server->Broadcast("Sending from server to client!\n");

    
}
