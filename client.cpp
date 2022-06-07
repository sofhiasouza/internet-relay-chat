#include "socket.h"

#include <iostream>

using namespace std;

#define SERVER_PORT 8192
#define SERVER_NAME "0.0.0.0"

Socket* client = new Socket(LOCALHOST_PORT);
bool quit = false;

void* send_thread(void* arg) {    
    int failed = 0;

    string message = "";    

    while (!quit) {
        do {
            cout << "To Server: ";
            getline(cin, message, '\n');
        } while (message.size() == 0); 

        cout << message << endl;

        if (message == "/quit") {
            cout << "Client left the server" << "\n";
            quit = true;

        } else {
            for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
                client->Write(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
            }
        }

        message = "";
    }
    cout << "saiu\n";
    pthread_exit(NULL);
}

void* receive_thread(void* arg) {    
    int failed = 0;
    int sender = *((int*)arg);

    while (!quit) {
        string message = client->Read(client->sockfd);
        cout << message << endl;
        message = "";
    }
    cout << "saiu\n";
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    client->Connect(SERVER_NAME, SERVER_PORT);
    
    string message = "";

    // while (message != "/quit") {

    //     /* Garante que todas as mensagens enviadas serão recebidas */        
    //     // do {
    //     message = socket->Read();

    //     cout << "From server: " << message << "\n";
    //     // } while (message.size() == Socket::buffer_size);

    //     // if (message.size() == 0) break;        

    //     /* Espera o usuário digitar uma mensagem válida para enviar */
    //     // do {
    //         cout << "To Server: ";
    //         getline(cin, message, '\n');
    //     // } while (message.size() == 0); 


    //     if (message == "/quit") break;

    //     /*Divide mensagens com mais de "buffer_size" caracteres e as envia em sequência*/
    //     // for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
    //     socket->Write(message);
    //         // socket->Check();
    //     // }
    // }

    pthread_t tid_send, tid_receive;

    if (pthread_create(&tid_send, NULL, send_thread, NULL) != 0) {
        cout << "Failed to create thread to send message" << endl;
        return 1;
    }
    if (pthread_create(&tid_receive, NULL, receive_thread, NULL) != 0) {
        cout << "Failed to create thread to receive message" << endl;
        return 1;
    }

    client->Disconnect();
    
    delete client;
    return 0;
}