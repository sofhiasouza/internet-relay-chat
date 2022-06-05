#include "socket.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

    Socket* server = new Socket(8192);
    server->Bind();

    cout << "Server Connected :)";

    server->Listen();

    string ip;
    int client;
    tie(client, ip) = server->Accept();

    send(client, "Sending from server to client!\n", 31, 0);

    while(1){
        
    }
}
