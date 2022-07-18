#include "socket.h"

#include <iostream>
#include <tuple>
#include <algorithm>
#include<vector>

#define SERVER_PORT 8193

using namespace std;

map<string, int> channels;

class Clients{

public:
    int id;
    string nick;
    string ip;
    bool is_channel = false;
    string channel_name;

};

vector<Clients> v_clients;

int find_pos(int id){

    for(int i=0;i<v_clients.size();i++){
        if(v_clients[i].id = id) return i;
    }

    return -1;

}
Socket* server = new Socket("0.0.0.0", SERVER_PORT);
int number_of_clients = 0;

void partitionateMessage(Socket * server, string message) {
    for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
        server->Broadcast(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
    }
}

void partitionateMessageCh(Socket * server, string message, int pos) {

    if(!v_clients[pos].is_channel){
        server->Write("Por favor, entre em um canal primeiro", v_clients[pos].id);
    }

    else{

        for(int j=0;j<v_clients.size();j++){
            if(v_clients[j].channel_name == v_clients[pos].channel_name){
                /*for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
                    server->Write(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)),v_clients[j].id);
                }*/

                
            }

            server->Broadcast(to_string(j)+" ");
            server->Broadcast(v_clients[j].nick+"\n");
        }

        //server->Broadcast("Isto é um teste");
    }

    //for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
      //  server->Broadcast(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
    //}
}


bool valid_ch_name(string ch_name){

    return true;
}

void *read_thread(void* arg){
    int failed = 0;
    bool quit = false;
    int client = *((int*)arg);
    int pos = find_pos(client);
    int client_id = ++number_of_clients;
    string name = v_clients[client_id-1].nick;

    //string message = "client#" + to_string(client_id) + " connected to the server";
    string message = "client " + name + " connected to the server";
    //cout << name << endl;
    cout << message << endl;
    partitionateMessage(server, message);

    while (!quit) {
        string original_message = server->Read(client);
        //message = "client#" + to_string(client_id) + ":" + original_message;
        message = "client " + name + ":" + original_message;
        cout << message << endl;

        if (original_message == "/quit") {
            //message = "client#" + to_string(client_id) + " left the server";
            message = "client " + name + " left the server";
            cout << message << endl;
            partitionateMessage(server, message);

            quit = true;
            server->Disconnect(client);
        }
        else if(original_message == "/ping"){
            server->Write("Server: pong", client);
        }
        else if(original_message.size() >= 6 and original_message.substr(0,5)=="/join"){
            string ch_name = original_message.substr(6, original_message.size());

            if(!valid_ch_name(ch_name)){
                server->Write("Nome não válido, tente novamente.", client);
                continue;
            }

            //já existe o canal?
            //int pos = find_pos(client);
            if(channels.find(ch_name) == channels.end()){ //aind não existe
                
                channels[ch_name] = v_clients[pos].id;
                v_clients[pos].is_channel = true;
                v_clients[pos].channel_name = ch_name;

                server->Write("Este é o nome do canal: "+ v_clients[pos].channel_name, client);
            }
            else{

                //channels[ch_name] = v_clients[pos].id;
                v_clients[pos].is_channel = true;
                v_clients[pos].channel_name = ch_name;

                //pos está indo para o primeiro
                server->Write("Você é o 2 ;D + " + v_clients[pos].nick, client);

            }

            //add ao canal
            




            //server->Write("Este é o nome do canal: "+ v_clients[pos].channel_name, client);

        } else {
            //partitionateMessage(server, message);
            //int pos = find_pos(client);
            partitionateMessageCh(server, message, pos);
        }
        message = "";
    }
    pthread_exit(NULL);
}

bool valid_nickname(string nickname){

    if(nickname.size()<3) return false;
    if(nickname == "server") return false;

    for(int i=0;i<nickname.size();i++){
        bool test = false;

        if(nickname[i] >= 'a' and nickname[i] <= 'z')
            test = true;
        else if(nickname[i] >= 'A' and nickname[i] <= 'Z')
            test = true;
        else if(nickname[i]=='.' or nickname[i]=='-' or nickname[i]=='_' )
            test = true;

        if(!test) return false; 
    } 

    return true;
}
string choose_nickname(int client){
    
    string message = "Choose your nickname: ";
    server->Write(message,client);
    string user_nickname = server->Read(client);

    while(!valid_nickname(user_nickname)){
        string message = "Nickname is not valid, please try again: ";
        server->Write(message, client);

        user_nickname = server->Read(client);
    }

    return user_nickname;
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

        //string nickname = server->Read(client);
        string nickname =  choose_nickname(client);
        Clients aux;
        aux.ip = ip;
        aux.nick = nickname;
        aux.id = client;
        v_clients.push_back(aux);

        

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
