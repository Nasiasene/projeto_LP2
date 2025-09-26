# Arquitetura do Sistema de Chat - Etapa 1

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

1. **Múltiplas threads** chamam métodos de log simultaneamente
2. **Mutex** garante que apenas uma thread acesse o write_internal() por vez
3. **write_internal()** formata a mensagem com timestamp e thread ID
4. **Saída** vai para console e/ou arquivo de forma segura

## Estrutura de Arquivos

```
include/
├── libtslog/
│   └── tslog.h          # API da biblioteca de logging
└── chat/                # Arquitetura para próximas etapas
    ├── types.h          # Tipos básicos (Message, Client)
    ├── server.h         # Interface do servidor TCP
    └── client.h         # Interface do cliente TCP

src/
└── libtslog/
    └── tslog.cpp        # Implementação thread-safe

tests/
└── test_libtslog.cpp    # Teste com 3 threads simultâneas
```

## Conceitos de Programação Concorrente Aplicados

### 1. **Threads** (std::thread)
- Múltiplas threads de execução simultânea
- Cada thread executa independentemente
- Compartilham o recurso de logging

### 2. **Exclusão Mútua** (std::mutex)
- Protege seção crítica (escrita de logs)
- Garante acesso exclusivo de uma thread por vez
- Implementado com std::lock_guard (RAII)

### 3. **Thread-Safety**
- Biblioteca segura para uso concorrente
- Sem race conditions na escrita
- Mensagens aparecem completas e íntegras

## Próximas Etapas

### Etapa 2: Cliente/Servidor Básico
- Implementação das classes Client e Server
- Comunicação TCP com sockets
- Uso da libtslog para logging de eventos

### Etapa 3: Sistema Completo
- Múltiplos clientes simultâneos
- Broadcast de mensagens
- Sincronização avançada (semáforos, condition variables)
- Monitores para estruturas compartilhadas
