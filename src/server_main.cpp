#include "chat/server.h"
#include <iostream>
#include <signal.h>

using namespace chat;

// Ponteiro global para o servidor 
ChatServer* server = nullptr;

void signal_handler(int sig) {
    if (server && sig == SIGINT) {
        std::cout << "\nRecebido sinal de interrupção. Parando servidor..." << std::endl;
        server->stop();
    }
}

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    
    // Permitir porta personalizada
    if (argc == 2) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cout << "Porta inválida! Usando porta padrão: " << DEFAULT_PORT << std::endl;
            port = DEFAULT_PORT;
        }
    }
    
    std::cout << "=== SERVIDOR DE CHAT TCP ===" << std::endl;
    std::cout << "Iniciando servidor na porta " << port << "..." << std::endl;
    std::cout << "Pressione Ctrl+C para parar o servidor" << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    // Criar servidor
    server = new ChatServer(port);
    
    // Configurar handler para Ctrl+C
    signal(SIGINT, signal_handler);
    
    // Iniciar servidor
    if (!server->start()) {
        std::cout << "ERRO: Não foi possível iniciar o servidor!" << std::endl;
        delete server;
        return 1;
    }
    
    // Aceitar conexões (loop principal)
    server->accept_connections();
    
    // Limpar
    delete server;
    std::cout << "Servidor finalizado." << std::endl;
    
    return 0;
}
