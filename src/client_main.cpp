#include "chat/client.h"
#include <iostream>

using namespace chat;

int main(int argc, char* argv[]) {
    std::string server_ip = "127.0.0.1";  // localhost por padrão
    int port = DEFAULT_PORT;
    std::string username;
    
    // Parâmetros: ./client [servidor] [porta] [usuario]
    if (argc >= 2) server_ip = argv[1];
    if (argc >= 3) port = std::atoi(argv[2]);
    if (argc >= 4) username = argv[3];
    
    // Se não forneceu usuário, perguntar
    if (username.empty()) {
        std::cout << "Digite seu nome de usuário: ";
        std::getline(std::cin, username);
        
        if (username.empty()) {
            std::cout << "Nome de usuário não pode estar vazio!" << std::endl;
            return 1;
        }
    }
    
    // Criar e executar cliente
    ChatClient client(server_ip, port, username);
    return client.run_cli();
}
