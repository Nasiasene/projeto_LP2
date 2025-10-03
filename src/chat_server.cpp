#include "chat/server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace chat;

ChatServer::ChatServer(int port) : port_(port), server_socket_(-1), running_(false) {
    logger_ = new tslog::TSLogger("server.log", true);
    logger_->info("Servidor criado na porta " + std::to_string(port));
}

ChatServer::~ChatServer() {
    stop();
    delete logger_;
}

bool ChatServer::start() {
    // Criar socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == -1) {
        logger_->error("Erro ao criar socket");
        return false;
    }

    // Permitir reutilizar endereço
    int opt = 1;
    setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configurar endereço
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    // Fazer bind
    if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        logger_->error("Erro no bind - porta pode estar em uso");
        close(server_socket_);
        return false;
    }

    // Escutar conexões
    if (listen(server_socket_, MAX_CLIENTS) == -1) {
        logger_->error("Erro ao escutar conexões");
        close(server_socket_);
        return false;
    }

    running_ = true;
    logger_->info("Servidor iniciado com sucesso na porta " + std::to_string(port_));
    
    return true;
}

void ChatServer::stop() {
    if (!running_) return;
    
    running_ = false;
    
    // Fechar conexões dos clientes
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (const auto& client : clients_) {
        close(client.socket_fd);
    }
    clients_.clear();
    
    // Fechar socket do servidor
    if (server_socket_ != -1) {
        close(server_socket_);
        server_socket_ = -1;
    }
    
    logger_->info("Servidor parado");
}

void ChatServer::accept_connections() {
    logger_->info("Aguardando conexões...");
    
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket == -1) {
            if (running_) {
                logger_->error("Erro ao aceitar conexão");
            }
            continue;
        }
        
        logger_->info("Nova conexão aceita: socket " + std::to_string(client_socket));
        
        // Criar thread para atender cliente
        std::thread client_thread(&ChatServer::handle_client, this, client_socket);
        client_thread.detach(); // Thread independente
    }
}

void ChatServer::handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    std::string username;
    
    // Receber nome do usuário
    ssize_t received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (received <= 0) {
        logger_->error("Erro ao receber nome do usuário");
        close(client_socket);
        return;
    }
    
    buffer[received] = '\0';
    username = std::string(buffer);
    
    // Remover quebras de linha do nome
    username.erase(std::remove(username.begin(), username.end(), '\n'), username.end());
    username.erase(std::remove(username.begin(), username.end(), '\r'), username.end());
    
    // Adicionar cliente à lista
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.push_back({username, client_socket});
    }
    
    logger_->info("Cliente conectado: " + username + " (socket " + std::to_string(client_socket) + ")");
    
    // Enviar mensagem de boas-vindas
    std::string welcome = "Bem-vindo ao chat, " + username + "!";
    send(client_socket, welcome.c_str(), welcome.length(), 0);
    
    // Loop para receber mensagens
    while (running_) {
        received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (received <= 0) {
            logger_->info("Cliente " + username + " desconectou");
            break;
        }
        
        buffer[received] = '\0';
        std::string message_content(buffer);
        
        // Criar mensagem com timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        
        Message msg = {username, message_content, timestamp.str()};
        
        // Salvar no histórico
        {
            std::lock_guard<std::mutex> lock(messages_mutex_);
            messages_.push_back(msg);
        }
        
        logger_->info("Mensagem de " + username + ": " + message_content);
        
        // Repassar para todos os outros clientes
        broadcast_message(msg, client_socket);
    }
    
    // Remover cliente quando desconectar
    remove_client(client_socket);
    close(client_socket);
}

void ChatServer::broadcast_message(const Message& msg, int sender_socket) {
    std::string formatted_msg = "[" + msg.timestamp + "] " + msg.username + ": " + msg.content;
    
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    // Enviar para todos os clientes, exceto o remetente
    for (auto it = clients_.begin(); it != clients_.end();) {
        if (it->socket_fd != sender_socket) {
            ssize_t sent = send(it->socket_fd, formatted_msg.c_str(), formatted_msg.length(), 0);
            
            if (sent == -1) {
                logger_->warning("Erro ao enviar mensagem para " + it->username);
            }
        }
        ++it;
    }
}

void ChatServer::remove_client(int client_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    auto it = std::find_if(clients_.begin(), clients_.end(),
        [client_socket](const Client& c) { return c.socket_fd == client_socket; });
    
    if (it != clients_.end()) {
        logger_->info("Removendo cliente: " + it->username);
        clients_.erase(it);
    }
}
