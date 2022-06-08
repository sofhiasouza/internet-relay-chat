#include "socket.h"

#include <iostream>
#include <tuple>
#include <algorithm>

#define SERVER_PORT 8193

using namespace std;

Socket* server = new Socket("0.0.0.0", SERVER_PORT);
int number_of_clients = 0;

void partitionateMessage(Socket * server, string message) {
    for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
        server->Broadcast(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
    }
}

void *read_thread(void* arg){
    int failed = 0;
    bool quit = false;
    int client = *((int*)arg);
    int client_id = ++number_of_clients;

    string message = "client#" + to_string(client_id) + " connected to the server";
    cout << message << endl;
    partitionateMessage(server, message);

    while (!quit) {
        string original_message = server->Read(client);
        message = "client#" + to_string(client_id) + ":" + original_message;
        cout << message << endl;

        if (original_message == "/quit") {
            message = "client#" + to_string(client_id) + " left the server";
            cout << message << endl;
            partitionateMessage(server, message);

            quit = true;
            server->Disconnect(client);
        } else {
            partitionateMessage(server, message);
        }
        message = "";
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){

    server->Bind();

    cout << "Server Connected :)\n";

    while(1) {
        server->Listen();

        string ip;
        int client;
        tie(client, ip) = server->Accept();

        server->Write("You are connected!\n", client);

        pthread_t tid;
        pthread_attr_t thread_attr;
    
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

        // Create thread to read messages from this new client
        if (pthread_create(&tid, &thread_attr, read_thread, &client) != 0) {
            cout << "Failed to create thread to send message" << endl;
        }
    }

    server->Disconnect();
}
