#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "types.h"
#include "../libtslog/tslog.h"
#include <string>
#include <thread>

namespace chat {

    // Cliente TCP simples para chat
    class ChatClient {
    private:
        std::string server_address_;
        int server_port_;
        std::string username_;
        
        int socket_fd_;
        bool connected_;
        
        std::thread* receive_thread_;
        tslog::TSLogger* logger_;

    public:
        ChatClient(const std::string& server_addr, int port, const std::string& username);
        ~ChatClient();
        
        bool connect();                   // Conecta ao servidor
        void disconnect();                // Desconecta
        bool is_connected() const { return connected_; }
        
        int run_cli();                    // Interface CLI
        bool send_message(const std::string& message);
        void receive_messages();          // Recebe mensagens
        void show_help() const;
        
        const std::string& get_username() const { return username_; }
    };

} // namespace chat

#endif // CHAT_CLIENT_H
