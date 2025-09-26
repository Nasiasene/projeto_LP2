#ifndef CHAT_TYPES_H
#define CHAT_TYPES_H

#include <string>

namespace chat {

    // Constantes básicas
    const int DEFAULT_PORT = 8080;
    const int MAX_CLIENTS = 50;
    const int BUFFER_SIZE = 1024;

    // Mensagem do chat
    struct Message {
        std::string username;
        std::string content;
        std::string timestamp;
    };

    // Cliente conectado
    struct Client {
        std::string username;
        int socket_fd;
    };

} // namespace chat

#endif // CHAT_TYPES_H
