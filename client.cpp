#include "socket.h"
#include <pthread.h>
#include<csignal>
//#include<stdio.h>

#include <iostream>

using namespace std;

#define SERVER_PORT 8193

Socket* client;
bool quit = false;

void Sigint_handler(int sig_num) {
    signal(SIGINT, Sigint_handler);
    cout << "\nCannot quit using crtl+c\n";
    fflush(stdout);
}

void* send_thread(void* arg) {    
    int failed = 0;

    string message = "";    

    while (!quit) {
        //getline(cin, message, '\n');

        if(getline(cin, message, '\n')){


            if (message == "/quit")
                quit = true;
                //break;

            for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
                client->Write(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
            }

            message = "";
        }
        else{
            quit = true;
            message = "/quit";

            for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
                client->Write(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
            }

            message = "";
        }

       
    }
    pthread_exit(NULL);
}

void* receive_thread(void* arg) {
    int failed = 0;
    string message = "";

    while (!quit) {
        do {
            message = client->Read(client->sockfd);
            cout << message  << endl;

            message = "";
        } while(message.size() ==  MAX_BUFFER_SIZE-1 && !quit);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    signal(SIGINT, Sigint_handler);
    
    if (argc < 2) {
        cout << "Provide your ip, please\n";
        return 1; 
    }

    string ip = argv[1];
    
    //Alteração
    string command = "";

    cout << "Conecte-se ao servidor digitando /connect.\n";

    while(command != "/connect"){
        if(!getline(cin, command, '\n')) {
            cout << "quitting" << endl;
            exit(0);
        }

        else if(command == "/quit")
            exit(0);

    }
    
    client = new Socket(ip, LOCALHOST_PORT);
    client->Connect(SERVER_PORT);

    string message = "";

    pthread_t tid_send, tid_receive;
    pthread_attr_t thread_attr;
    void * status;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    //Create thread to watch messages sent by this client
    if (pthread_create(&tid_send, &thread_attr, send_thread, NULL) != 0) {
        cout << "Failed to create thread to send message" << endl;
        return 1;
    }

    //Create thread to watch messages received by this client
    if (pthread_create(&tid_receive, &thread_attr, receive_thread, NULL) != 0) {
        cout << "Failed to create thread to receive message" << endl;
        return 1;
    }

    pthread_attr_destroy(&thread_attr);

    if(pthread_join(tid_send, &status)) {
        cout << "Failed to join send thread" << endl;
        exit(1);
    }
    pthread_cancel(tid_receive);
    if(pthread_join(tid_receive, &status)) {
        cout << "Failed to join receive thread" << endl;
        exit(1);
    }

    client->Disconnect();

    cout << "Client has disconnected from the server" << endl;
    
    delete client;
    return 0;
}
