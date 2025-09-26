#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "types.h"
#include "../libtslog/tslog.h"
#include <vector>
#include <thread>
#include <mutex>

namespace chat {

    // Servidor TCP simples para chat multiusuário
    class ChatServer {
    private:
        int port_;
        int server_socket_;
        bool running_;
        
        std::vector<Client> clients_;        // Lista de clientes conectados
        std::vector<Message> messages_;      // Histórico de mensagens
        
        std::mutex clients_mutex_;           // Protege lista de clientes
        std::mutex messages_mutex_;          // Protege histórico
        
        tslog::TSLogger* logger_;

    public:
        explicit ChatServer(int port = DEFAULT_PORT);
        ~ChatServer();
        
        bool start();                        // Inicia servidor
        void stop();                         // Para servidor
        bool is_running() const { return running_; }
        
        void accept_connections();           // Aceita novos clientes
        void handle_client(int client_socket); // Atende um cliente
        void broadcast_message(const Message& msg, int sender_socket = -1);
        void remove_client(int client_socket);
    };

} // namespace chat

#endif // CHAT_SERVER_H
