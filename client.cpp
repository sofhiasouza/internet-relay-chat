#include "socket.h"

#include <iostream>

using namespace std;

#define SERVER_PORT 8192
#define SERVER_NAME "cool_chat"

int main(int argc, char* argv[]) {

    Socket* socket = new Socket(LOCALHOST_PORT);

    socket->Connect(SERVER_NAME, SERVER_PORT);
    
    string message = "";

    while (message != "/quit") {

        /* Garante que todas as mensagens enviadas serão recebidas */        
        // do {
        message = socket->Read();

        cout << "From server: " << message << "\n";
        // } while (message.size() == Socket::buffer_size);

        // if (message.size() == 0) break;        

        /* Espera o usuário digitar uma mensagem válida para enviar */
        // do {
            cout << "To Server: ";
            getline(cin, message, '\n');
        // } while (message.size() == 0); 


        if (message == "/quit") break;

        /*Divide mensagens com mais de "buffer_size" caracteres e as envia em sequência*/
        // for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
        socket->Write(message);
            // socket->Check();
        // }
    }

    socket->Disconnect();
    
    delete socket;
    return 0;
}