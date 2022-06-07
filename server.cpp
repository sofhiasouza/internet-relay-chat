#include "socket.h"

#include <iostream>
#include <tuple>
#include <algorithm>

#define SERVER_PORT 8192

using namespace std;

Socket* server = new Socket(SERVER_PORT);


int main(int argc, char* argv[]){

    server->Bind();

    cout << "Server Connected :)\n";

    server->Listen();

    string ip;
    int client;
    tie(client, ip) = server->Accept();

    server->Broadcast("Sending from server to client!\n");

    int failed = 0;
    bool quit = false;

    while (!quit) {
        cout << "aqui\n";
        string message = server->Read(client);
        cout << message << endl;

        if (message == "/quit") {
            cout << "Client left the server" << "\n";
            quit = true;

        } else {
            for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
                server->Broadcast(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
            }
        }
        message = "";
    }
    cout << "saiu\n";
}
