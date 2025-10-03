#include "chat/client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace chat;

ChatClient::ChatClient(const std::string& server_addr, int port, const std::string& username)
    : server_address_(server_addr), server_port_(port), username_(username),
      socket_fd_(-1), connected_(false), receive_thread_(nullptr) {
    logger_ = new tslog::TSLogger("client_" + username + ".log", true);
    logger_->info("Cliente criado: " + username + " -> " + server_addr + ":" + std::to_string(port));
}

ChatClient::~ChatClient() {
    disconnect();
    delete logger_;
}

bool ChatClient::connect() {
    // Criar socket
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ == -1) {
        logger_->error("Erro ao criar socket");
        return false;
    }

    // Configurar endereço do servidor
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_);
    
    if (inet_pton(AF_INET, server_address_.c_str(), &server_addr.sin_addr) <= 0) {
        logger_->error("Endereço IP inválido: " + server_address_);
        close(socket_fd_);
        return false;
    }

    // Conectar ao servidor
    if (::connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        logger_->error("Erro ao conectar ao servidor");
        close(socket_fd_);
        return false;
    }

    // Enviar nome de usuário
    ssize_t sent = send(socket_fd_, username_.c_str(), username_.length(), 0);
    if (sent == -1) {
        logger_->error("Erro ao enviar nome de usuário");
        close(socket_fd_);
        return false;
    }

    connected_ = true;
    logger_->info("Conectado ao servidor com sucesso!");

    // Iniciar thread para receber mensagens
    receive_thread_ = new std::thread(&ChatClient::receive_messages, this);

    return true;
}

void ChatClient::disconnect() {
    if (!connected_) return;

    connected_ = false;

    // Fechar socket
    if (socket_fd_ != -1) {
        close(socket_fd_);
        socket_fd_ = -1;
    }

    // Aguardar thread de recepção terminar
    if (receive_thread_) {
        if (receive_thread_->joinable()) {
            receive_thread_->join();
        }
        delete receive_thread_;
        receive_thread_ = nullptr;
    }

    logger_->info("Desconectado do servidor");
}

int ChatClient::run_cli() {
    std::cout << "\n=== CHAT CLIENT - " << username_ << " ===" << std::endl;
    std::cout << "Conectando ao servidor " << server_address_ << ":" << server_port_ << "..." << std::endl;

    if (!connect()) {
        std::cout << "ERRO: Não foi possível conectar ao servidor!" << std::endl;
        return 1;
    }

    std::cout << "Conectado! Digite suas mensagens (ou 'quit' para sair, 'help' para ajuda):" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    std::string input;
    while (connected_ && std::getline(std::cin, input)) {
        if (input.empty()) continue;

        if (input == "quit" || input == "exit") {
            std::cout << "Saindo do chat..." << std::endl;
            break;
        }

        if (input == "help") {
            show_help();
            continue;
        }

        if (!send_message(input)) {
            std::cout << "ERRO: Não foi possível enviar mensagem!" << std::endl;
            break;
        }
    }

    disconnect();
    return 0;
}

bool ChatClient::send_message(const std::string& message) {
    if (!connected_) return false;

    ssize_t sent = send(socket_fd_, message.c_str(), message.length(), 0);
    if (sent == -1) {
        logger_->error("Erro ao enviar mensagem: " + message);
        return false;
    }

    logger_->info("Mensagem enviada: " + message);
    return true;
}

void ChatClient::receive_messages() {
    char buffer[BUFFER_SIZE];

    while (connected_) {
        ssize_t received = recv(socket_fd_, buffer, BUFFER_SIZE - 1, 0);

        if (received <= 0) {
            if (connected_) {
                logger_->error("Conexão perdida com o servidor");
                connected_ = false;
                std::cout << "\n[CONEXÃO PERDIDA]" << std::endl;
            }
            break;
        }

        buffer[received] = '\0';
        std::string message(buffer);

        logger_->info("Mensagem recebida: " + message);
        std::cout << message << std::endl;
    }
}

void ChatClient::show_help() const {
    std::cout << "\n=== AJUDA DO CHAT ===" << std::endl;
    std::cout << "Comandos disponíveis:" << std::endl;
    std::cout << "  help  - Mostra esta ajuda" << std::endl;
    std::cout << "  quit  - Sai do chat" << std::endl;
    std::cout << "  exit  - Sai do chat" << std::endl;
    std::cout << "\nPara enviar mensagem, digite normalmente e pressione Enter." << std::endl;
    std::cout << std::string(50, '-') << std::endl;
}
