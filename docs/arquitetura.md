# Arquitetura do Sistema

## Diagrama da Biblioteca libtslog

```
┌─────────────────────────────────────────────────────────┐
│                  APLICAÇÃO                              │
│                                                         │
│  Thread 1    Thread 2    Thread 3    ...  Thread N     │
│     │           │           │               │          │
│     ▼           ▼           ▼               ▼          │
│   debug()     info()    warning()        error()       │
│     │           │           │               │          │
│     └───────────┼───────────┼───────────────┘          │
│                 │           │                          │
└─────────────────┼───────────┼──────────────────────────┘
                  │           │
┌─────────────────┼───────────┼──────────────────────────┐
│              LIBTSLOG       │                          │
│                 │           │                          │
│            ┌────▼───────────▼─────┐                    │
│            │   std::mutex         │                    │
│            │   (exclusão mútua)   │                    │
│            └──────────┬───────────┘                    │
│                       │                                │
│            ┌──────────▼────────────┐                   │
│            │   write_internal()    │                   │
│            │   [THREAD-SAFE]       │                   │
│            └─────┬─────────────┬───┘                   │
│                  │             │                       │
│         ┌────────▼─┐      ┌────▼─────┐                 │
│         │ Console  │      │ Arquivo  │                 │
│         │ (stdout) │      │test.log  │                 │
│         └──────────┘      └──────────┘                 │
└─────────────────────────────────────────────────────────┘
```

## Fluxo de Execução

1. Múltiplas threads chamam métodos de log simultaneamente
2. Mutex garante que apenas uma thread acesse write_internal() por vez
3. write_internal() formata a mensagem com timestamp e thread ID
4. Saída vai para console e/ou arquivo de forma segura

## Estrutura de Arquivos

```
include/
├── libtslog/
│   └── tslog.h          # API da biblioteca de logging
└── chat/                # Headers do sistema de chat
    ├── types.h          # Tipos básicos (Message, Client)
    ├── server.h         # Interface do servidor TCP
    └── client.h         # Interface do cliente TCP

src/
├── libtslog/
│   └── tslog.cpp        # Implementação thread-safe
├── chat_server.cpp      # Servidor TCP
├── chat_client.cpp      # Cliente CLI
├── server_main.cpp      # Executável servidor
└── client_main.cpp      # Executável cliente

tests/
├── test_libtslog.cpp    # Teste com 3 threads simultâneas
└── test_chat.sh         # Script teste múltiplos clientes
```

## Conceitos de Programação Concorrente

### Threads (std::thread)

- Múltiplas threads de execução simultânea
- Cada thread executa independentemente
- Compartilham recursos de logging e rede

### Exclusão Mútua (std::mutex)

- Protege seções críticas
- Garante acesso exclusivo de uma thread por vez
- Implementado com std::lock_guard (RAII)

### Thread-Safety

- Biblioteca segura para uso concorrente
- Sem race conditions na escrita
- Mensagens aparecem completas e íntegras

## Diagramas de Sequência

### Cliente-Servidor Básico

```
Cliente                 Servidor
   |                       |
   |----connect()---------->|
   |                       |--create_thread()-->Thread
   |<---welcome_msg---------|                      |
   |                       |                      |
   |----send_message()------|--------------------->|
   |                       |                      |--broadcast()
   |<---receive_message()---|<---------------------|
   |                       |                      |
   |----disconnect()--------|--------------------->|
   |                       |                      |--cleanup()
```
