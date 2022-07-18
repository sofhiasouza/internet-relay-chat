#include "socket.h"

#include <iostream>
#include <tuple>
#include <algorithm>
#include<vector>
#include <pthread.h>

#define SERVER_PORT 8193

using namespace std;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
map<string, int> channels;
map<string, int> nicknames;

class Clients{

public:
    int id;
    string nick;
    string ip;
    bool is_channel = false;
    bool is_admin = false;
    bool is_muted = false;
    string channel_name;

};

vector<Clients> v_clients;

int find_client(string nickname){

    for(int i=0;i<v_clients.size();i++){
        if(v_clients[i].nick == nickname) return i;
    }

    return -1;

}
Socket* server = new Socket("0.0.0.0", SERVER_PORT);
int number_of_clients = 0;

void partitionateMessage(Socket * server, string message) {
    pthread_mutex_lock(&lock);
    for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
        server->Broadcast(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)));
    }
    pthread_mutex_unlock(&lock);
}

void partitionateMessageCh(Socket * server, string message, int pos) {
    pthread_mutex_lock(&lock);

    if(!v_clients[pos].is_channel){
        server->Write("Please, join a channel first", v_clients[pos].id);
    }

    else{

        for(int j=0;j<v_clients.size();j++){
            if(v_clients[j].channel_name == v_clients[pos].channel_name){
                for(int i = 0 ; i < message.size() ; i += MAX_BUFFER_SIZE-1){
                    server->Write(message.substr(i, min(MAX_BUFFER_SIZE-1, (int)message.size()-i+1)),v_clients[j].id);
                }
            }
        }
    }

    pthread_mutex_unlock(&lock);
}


bool valid_ch_name(string ch_name){
    
    if(ch_name.size()<3) return false;
    if(ch_name == "server") return false;

    for(int i=0;i<ch_name.size();i++){
        if(ch_name[i] == 7 || ch_name[i] == ',' || ch_name[i] == ' ') return false;    
    } 
    return true;
}


bool valid_nickname(string nickname){

    if(nickname.size()<3 || nickname.size() > 50) return false;
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

    if(nicknames.count(nickname)) return false;

    return true;
}

void *read_thread(void* arg){
    int failed = 0;
    bool quit = false;
    int client = *((int*)arg);
    int client_id = ++number_of_clients;
    int pos = client_id-1;
    string name = v_clients[client_id-1].nick;

    //string message = "client#" + to_string(client_id) + " connected to the server";
    string message = "client " + name + " connected to the server";
    cout << message << endl;
    partitionateMessage(server, message);

    while (!quit) {
        name = v_clients[client_id-1].nick;
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
            string ch_name = "#" + original_message.substr(6, original_message.size());

            cout << ch_name << "\n";
            if(!valid_ch_name(ch_name)){
                server->Write("Invalid name, please try again.", client);
                continue;
            }

            //já existe o canal?
            if(channels.find(ch_name) == channels.end()){ //aind não existe
                
                channels[ch_name] = v_clients[pos].id;
                v_clients[pos].is_channel = true;
                v_clients[pos].channel_name = ch_name;
                v_clients[pos].is_admin = true;
                v_clients[pos].is_muted = false;

                server->Write("This is the channel name: "+ v_clients[pos].channel_name, client);
                server->Write("\nYou are the channel admin!", client);
            }
            else{

                //channels[ch_name] = v_clients[pos].id;
                v_clients[pos].is_channel = true;
                v_clients[pos].channel_name = ch_name;
                v_clients[pos].is_muted = false;

                server->Write("Welcome to channel " + ch_name + "! You can already send messages.", client);

            }

        } 
        else if(original_message.size() >= 10 and original_message.substr(0,9)=="/nickname"){
            string new_nick = original_message.substr(10, original_message.size());

            if(!valid_nickname(new_nick)){
                server->Write("Nickname not valid.", client);
            }
            else{
                nicknames.erase(v_clients[pos].nick);
                nicknames[new_nick] = 1;
                v_clients[pos].nick = new_nick;
                server->Write("Now your nickname is " + new_nick, client);
            }
        }
        else if(original_message.size() >= 6 and original_message.substr(0,5)=="/kick"){

            if(!v_clients[pos].is_admin){
                server->Write("Sorry, only admins can perfom this action :(", client);
            }
            else{
                string nick = original_message.substr(6, original_message.size());
                int kick_pos = -1;

                // search for user in channel
                for(int i=0;i<v_clients.size();i++){
                    if(v_clients[i].nick == nick && v_clients[i].channel_name == v_clients[pos].channel_name) kick_pos = i;
                }

                if(kick_pos == -1){
                    server->Write("User doesn't exists or is not in this channel", client);
                }
                else{
                    v_clients[kick_pos].is_channel = false;
                    v_clients[kick_pos].channel_name = "";
                    server->Write("Admin kicked you out", v_clients[kick_pos].id);
                    server->Write("User disconnected", client);
                }

            }
        }
        else if(original_message.size() >= 6 and original_message.substr(0,5)=="/mute"){

            if(!v_clients[pos].is_admin){
                server->Write("Sorry, only admins can perfom this action :(", client);
            }
            else{
                string nick = original_message.substr(6, original_message.size());
                int kick_pos = -1;

                // search for user in channel
                for(int i=0;i<v_clients.size();i++){
                    if(v_clients[i].nick == nick && v_clients[i].channel_name == v_clients[pos].channel_name) kick_pos = i;
                }

                if(kick_pos == -1){
                    server->Write("User doesn't exists or is not in this channel", client);
                }
                else{
                    if(v_clients[kick_pos].is_muted){
                        server->Write("User is already muted", client);
                    }
                    else {
                        v_clients[kick_pos].is_muted = true;
                        server->Write("Admin muted you", v_clients[kick_pos].id);
                        server->Write("User muted", client);
                    }
                }

            }
        }
        else if(original_message.size() >= 8 and original_message.substr(0,7)=="/unmute"){

            if(!v_clients[pos].is_admin){
                server->Write("Sorry, only admins can perfom this action :(", client);
            }
            else{
                string nick = original_message.substr(8, original_message.size());
                int kick_pos = -1;

                // search for user in channel
                for(int i=0;i<v_clients.size();i++){
                    if(v_clients[i].nick == nick && v_clients[i].channel_name == v_clients[pos].channel_name) kick_pos = i;
                }

                if(kick_pos == -1){
                    server->Write("User doesn't exists or is not in this channel", client);
                }
                else{

                    if(!v_clients[kick_pos].is_muted){
                        server->Write("User is not muted", client);
                    }
                    else {
                        v_clients[kick_pos].is_muted = false;
                        server->Write("Admin unmuted you", v_clients[kick_pos].id);
                        server->Write("User unmuted", client);
                    }
                }

            }
        }
        else if(original_message.size() >= 7 and original_message.substr(0,6)=="/whois"){

            if(!v_clients[pos].is_admin){
                server->Write("Sorry, only admins can perfom this action :(", client);
            }
            else{
                string nick = original_message.substr(7, original_message.size());
                int kick_pos = -1;

                // search for user in channel
                for(int i=0;i<v_clients.size();i++){
                    if(v_clients[i].nick == nick && v_clients[i].channel_name == v_clients[pos].channel_name) kick_pos = i;
                }

                if(kick_pos == -1){
                    server->Write("User doesn't exists or is not in this channel", client);
                }
                else{
                    server->Write("The user ip is " + v_clients[kick_pos].ip , client);
                }

            }
        }
        else {
            if(v_clients[pos].is_muted){
                server->Write("Admin muted you, so you can't send messages in this channel.", client);
            }
            else partitionateMessageCh(server, message, pos);
        }
        message = "";
    }
    pthread_exit(NULL);
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

        pthread_mutex_lock(&lock);
        Clients aux;
        aux.ip = ip;
        aux.nick = nickname;
        aux.id = client;
        v_clients.push_back(aux);
        pthread_mutex_unlock(&lock);

        nicknames[nickname] = 1;

        

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
