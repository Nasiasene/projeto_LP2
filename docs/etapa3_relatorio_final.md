# Relatório Final - Etapa 3: Sistema Completo

**Data**: 04/10/2025  
**Tag**: v3-final  
**Status**: ✅ CONCLUÍDA

## Sistema de Chat Multiusuário TCP - Funcionalidades Obrigatórias

### ✅ Requisitos Atendidos (Tema A)

- ✅ **Servidor TCP concorrente** aceitando múltiplos clientes
- ✅ **Thread por cliente** - cada cliente atendido por thread separada
- ✅ **Broadcast de mensagens** - retransmitidas para todos os demais clientes
- ✅ **Logging concorrente** usando biblioteca libtslog desenvolvida
- ✅ **Cliente CLI** - conectar, enviar e receber mensagens
- ✅ **Proteção de estruturas compartilhadas** - lista de clientes e histórico protegidos

## Diagrama de Sequência Cliente-Servidor

```
   Cliente João      Servidor Main      Thread Cliente     Cliente Ana        Logging
        |                  |                  |                |                |
        |                  |                  |                |                |
 [1]    |--- connect() --->|                  |                |                |
        |<--- accept() ----|                  |                |                |
        |                  |                  |                |                |
        |                  |-- create_thread->|                |                |
        |                  |                  |- log("conexão")---------------> |
        |                  |                  |                |                |
 [2]    |--- send("João")----------------->|                   |                |
        |                  |                  |                |                |
        |                  |                  |- add_client()--|                |
        |                  |                  |- log("João entrou")-----------> |
        |                  |                  |                |                |
 [3]    |--- send("Olá!")-------------------->|                |                |
        |                  |                  |                |                |
        |                  |                  |- save_msg()----|                |
        |                  |                  |- log("msg João")--------------> |
        |                  |                  |                |                |
        |                  |                  |- broadcast()-->|                |
        |                  |                  |  "[12:30] João: Olá!"           |
        |                  |                  |                |                |
 [4]    |                  |                  |<-- send("Oi")- |                |
        |                  |                  |                |                |
        |                  |                  |- save_msg()----|                |
        |                  |                  |- log("msg Ana")---------------> |
        |                  |                  |                |                |
        |<-- broadcast()---|                  |                |                |
        |  "[12:31] Ana: Oi"                  |                |                |
        |                  |                  |                |                |
 [5]    |--- disconnect--->|                  |                |                |
        |                  |                  |- remove_client()                |
        |                  |                  |- log("João saiu")-------------> |
        |                  |<-- thread_exit---|                |                |
        X                  |                  X                |                |
```

### Componentes do Sistema:

| Componente           | Responsabilidade                                |
| -------------------- | ----------------------------------------------- |
| **Cliente João/Ana** | Interface CLI, conexão TCP, envio/recepção      |
| **Servidor Main**    | Accept de conexões, gerenciamento de threads    |
| **Thread Cliente**   | Handler dedicado, broadcast, gerência de estado |
| **Logging**          | Sistema thread-safe de logs (libtslog)          |

### Fluxo Detalhado:

**[1] Conexão TCP:**

- Cliente cria socket e conecta ao servidor
- Servidor aceita conexão na thread principal
- Log de nova conexão é registrado

**[2] Autenticação:**

- Cliente envia nome de usuário
- Servidor cria thread dedicada para o cliente
- Cliente é adicionado à lista de clientes ativos
- Entrada do usuário é logada

**[3] Envio de Mensagem:**

- Cliente envia mensagem através de sua thread
- Servidor adiciona ao histórico protegido por mutex
- Mensagem é logada com timestamp
- Broadcast é feito para todos os outros clientes

**[4] Recepção de Mensagens:**

- Outro cliente envia mensagem
- Servidor processa e faz broadcast
- Todos os clientes (exceto remetente) recebem a mensagem
- Operação é logada

**[5] Desconexão:**

- Cliente fecha conexão (graceful ou abrupta)
- Servidor detecta desconexão na thread do cliente
- Cliente é removido da lista (protegida por mutex)
- Thread do cliente é finalizada
- Desconexão é logada

## Mapeamento Requisitos → Código

### 1. Threads (std::thread)

```cpp
// Arquivo: src/chat_server.cpp:95-105
void ChatServer::accept_connections() {
    while (running_) {
        int client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket != -1) {
            std::thread client_thread(&ChatServer::handle_client, this, client_socket);
            client_thread.detach(); // Thread independente para cada cliente
        }
    }
}
```

### 2. Exclusão Mútua (std::mutex)

```cpp
// Arquivo: include/chat/server.h:22-23
std::mutex clients_mutex_;  // Protege lista de clientes
std::mutex messages_mutex_; // Protege histórico

// Uso: src/chat_server.cpp:158-161
{
    std::lock_guard<std::mutex> lock(messages_mutex_);
    messages_.push_back(msg);
}
```

### 3. Sockets TCP

```cpp
// Arquivo: src/chat_server.cpp:24-42
server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr));
listen(server_socket_, MAX_CLIENTS);
```

### 4. Logging Thread-Safe (libtslog)

```cpp
// Arquivo: include/libtslog/tslog.h:25-30
class TSLogger {
    std::mutex log_mutex_;
    void log(LogLevel level, const std::string& message);
};
```

### 5. Estruturas Compartilhadas Protegidas

```cpp
// Arquivo: src/chat_server.cpp:174-190
void ChatServer::broadcast_message(const Message& msg, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto it = clients_.begin(); it != clients_.end();) {
        if (it->socket_fd != sender_socket) {
            send(it->socket_fd, formatted_msg.c_str(), formatted_msg.length(), 0);
        }
        ++it;
    }
}
```

## Conceitos de Concorrência Implementados

### 1. **Threads**: `std::thread`

- **Onde**: Servidor cria uma thread para cada cliente conectado
- **Por que**: Permite atendimento simultâneo de múltiplos clientes
- **Como**: `std::thread client_thread(&ChatServer::handle_client, this, socket)`

### 2. **Exclusão Mútua**: `std::mutex + std::lock_guard`

- **Onde**: Proteção das listas compartilhadas (clientes, mensagens)
- **Por que**: Evita race conditions no acesso concorrente
- **Como**: RAII com `std::lock_guard<std::mutex> lock(mutex_)`

### 3. **Sockets TCP**: Comunicação em rede

- **Onde**: Servidor escuta conexões, clientes conectam
- **Por que**: Comunicação cliente-servidor através da rede
- **Como**: API Berkeley Sockets (socket, bind, listen, accept)

### 4. **Thread-Safe Logging**: Biblioteca libtslog

- **Onde**: Múltiplas threads escrevendo logs simultaneamente
- **Por que**: Debug e monitoramento sem conflitos
- **Como**: Mutex interno protegendo escrita em arquivo

### 5. **RAII**: Gerenciamento automático de recursos

- **Onde**: `std::lock_guard`, sockets, threads
- **Por que**: Liberação automática, código mais seguro
- **Como**: Destructors automáticos em C++

## Arquivos e Responsabilidades

| Arquivo                    | Responsabilidade         | Conceitos               |
| -------------------------- | ------------------------ | ----------------------- |
| `include/libtslog/tslog.h` | API logging thread-safe  | Mutex, RAII             |
| `src/libtslog/tslog.cpp`   | Implementação logging    | Exclusão mútua          |
| `include/chat/server.h`    | Interface servidor TCP   | Threads, Mutex          |
| `src/chat_server.cpp`      | Servidor multithread     | Threads, Sockets, Mutex |
| `include/chat/client.h`    | Interface cliente TCP    | Sockets                 |
| `src/chat_client.cpp`      | Cliente thread-safe      | Threads, Sockets        |
| `src/server_main.cpp`      | Executável servidor      | Gerenciamento recursos  |
| `src/client_main.cpp`      | Executável cliente       | CLI, Sockets            |
| `tests/test_chat.sh`       | Teste múltiplos clientes | Concorrência            |

## Estatísticas do Projeto

- **Linhas de código**: ~800
- **Arquivos C++**: 8
- **Threads simultâneas**: 1 servidor + N clientes (máx 50)
- **Recursos protegidos**: 2 (lista clientes + histórico)
