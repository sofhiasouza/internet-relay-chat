#include "socket.h"
#include <pthread.h>

#include <iostream>

using namespace std;

#define SERVER_PORT 8192

char SERVER_NAME[] = "127.0.0.1";

Socket* client;
bool quit = false;

void* send_thread(void* arg) {    
    int failed = 0;

    string message = "";    

    while (!quit) {
        cout << "To Server: ";
        getline(cin, message, '\n');

        if (message == "/quit") {
            quit = true;
        } else {
            for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
                client->Write(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
            }
        }

        message = "";
    }
    cout << "Saiu da thread send\n";
    pthread_exit(NULL);
}

void* receive_thread(void* arg) {   
    cout << "Comecando received thread" << endl; 
    int failed = 0;
    // int sender = *(int*) arg;

    cout << "Deu bom ate aqui" << endl;

    while (!quit) {
        string message = client->Read(client->sockfd);
        cout << "From server: " << message;
        message = "";
    }
    cout << "Saiu da thread receive" << endl;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    client = new Socket(LOCALHOST_PORT);
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
    pthread_attr_t thread_attr;
    void * status;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    if (pthread_create(&tid_send, &thread_attr, send_thread, NULL) != 0) {
        cout << "Failed to create thread to send message" << endl;
        return 1;
    }
    if (pthread_create(&tid_receive, &thread_attr, receive_thread, NULL) != 0) {
        cout << "Failed to create thread to receive message" << endl;
        return 1;
    }

    pthread_attr_destroy(&thread_attr);

    if(!pthread_join(tid_send, &status)) {
        cout << "Failed to join send thread" << endl;
        exit(1);
    }
    if(!pthread_join(tid_receive, &status)) {
        cout << "Failed to join receive thread" << endl;
        exit(1);
    }

    client->Disconnect();

    cout << "Client has disconnected from the server" << endl;
    
    delete client;
    return 0;
}
