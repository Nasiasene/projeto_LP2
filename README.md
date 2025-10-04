# Projeto LP2 - Chat Multiusuário TCP

**Tema:** Servidor de Chat Multiusuário (TCP)  
## Início Rápido

### Compilar

```bash
make all    # Compila tudo (servidor + cliente + testes)
make clean  # Limpa arquivos
```

### Executar

```bash
# Terminal 1 - Servidor
make server              # Porta padrão (8080)
./build/server 9999      # Porta personalizada

# Terminal 2 - Cliente
make client                           # Pergunta usuário
./build/client 127.0.0.1 8080 João    # IP, porta e usuário
```

### Teste Automático

```bash
./tests/test_chat.sh     # 3 clientes simultâneos
./tests/test_chat.sh 5   # 5 clientes simultâneos
```

## Conceitos de Concorrência Usados

- **Threads**: `std::thread` - uma por cliente conectado
- **Exclusão mútua**: `std::mutex` + `std::lock_guard` para listas compartilhadas
- **Sockets TCP**: Comunicação cliente-servidor em rede
- **Thread-safe logging**: Múltiplas threads escrevendo logs simultaneamente
- **RAII**: Gerenciamento automático de recursos

## Estrutura do Projeto

```
projeto_lp2/
├── include/
│   ├── libtslog/tslog.h       # API logging thread-safe
│   └── chat/                  # Headers do sistema de chat
├── src/
│   ├── libtslog/tslog.cpp     # Implementação logging
│   ├── chat_server.cpp        # Servidor TCP
│   ├── chat_client.cpp        # Cliente CLI
│   ├── server_main.cpp        # Executável servidor
│   └── client_main.cpp        # Executável cliente
├── docs/                      # Documentação detalhada
├── tests/test_libtslog.cpp    # Teste da biblioteca
├── tests/test_chat.sh         # Script teste múltiplos clientes
└── Makefile                  # Sistema de build
```

## Comandos do Cliente

- `help` - Mostra ajuda
- `quit` ou `exit` - Sair do chat
- Qualquer texto - Enviar mensagem

## Especificações Técnicas

- **Porta padrão:** 8080
- **Máximo de clientes:** 50 (configurável em `types.h`)
- **Buffer de mensagem:** 1024 bytes
- **Protocolo:** TCP
- **Thread-safe:** Sim (libtslog + mutexes)
- **Logs automáticos:** server.log, client\_[usuario].log

## Documentação Detalhada

Consulte a pasta `docs/` para:

- **Arquitetura do sistema** (`arquitetura.md`)
- **Análise crítica com IA** (`analise_critica_ia.md`)
- **Relatórios de todas as etapas** (`etapa1_*.md`, `etapa2_*.md`)
- **Relatório final da Etapa 3** (`etapa3_relatorio_final.md`)
- **Diagrama de sequência** e mapeamento requisitos→código

## Status do Projeto

✅ **Etapa 1** - Biblioteca libtslog + Arquitetura (Tag: v1-logging)  
✅ **Etapa 2** - Protótipo CLI de Comunicação (Tag: v2-cli)  
✅ **Etapa 3** - Sistema Completo (Tag: v3-final)

**Funcionalidades Obrigatórias do Tema A - Chat Multiusuário TCP:**

- ✅ Servidor TCP concorrente aceitando múltiplos clientes
- ✅ Cada cliente atendido por thread separada
- ✅ Mensagens retransmitidas para todos os clientes (broadcast)
- ✅ Logging concorrente usando libtslog
- ✅ Cliente CLI para conectar, enviar e receber mensagens
- ✅ Proteção de estruturas compartilhadas (lista de clientes, histórico)
