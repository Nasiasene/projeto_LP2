# Relatório Etapa 2: Protótipo CLI de Comunicação

**Data**: 03/10/2025  
**Tag**: v2-cli  
**Status**: ✅ CONCLUÍDA

## Requisitos Atendidos

### Funcionalidades Obrigatórias

- ✅ Servidor TCP concorrente aceitando múltiplos clientes
- ✅ Cada cliente atendido por thread separada
- ✅ Mensagens retransmitidas para todos os clientes (broadcast)
- ✅ Logging concorrente integrado usando libtslog
- ✅ Cliente CLI para conectar, enviar e receber mensagens
- ✅ Proteção de estruturas compartilhadas com mutex
- ✅ Scripts de teste (simulação de múltiplos clientes)

### Conceitos de Concorrência Implementados

- ✅ Threads: std::thread para cada cliente conectado
- ✅ Exclusão mútua: std::mutex protegendo listas compartilhadas
- ✅ Sockets TCP: Comunicação cliente-servidor em rede
- ✅ Thread-safe logging: Múltiplas threads escrevendo logs simultaneamente
- ✅ RAII: Gerenciamento automático de recursos

## Implementação Técnica

### Arquivos Criados

- src/chat_server.cpp - Implementação do servidor TCP
- src/chat_client.cpp - Implementação do cliente CLI
- src/server_main.cpp - Executável do servidor
- src/client_main.cpp - Executável do cliente
- tests/test_chat.sh - Script para teste com múltiplos clientes
- Makefile atualizado

### Sincronização

- **clients*mutex***: Protege lista de clientes conectados
- **messages*mutex***: Protege histórico de mensagens
- **libtslog**: Mutex interno para escrita segura de logs
- **std::lock_guard**: Liberação automática de locks

### Protocolo de Comunicação

1. Cliente conecta via TCP
2. Envia nome de usuário
3. Servidor cria thread dedicada
4. Loop de recepção de mensagens
5. Broadcast para todos os outros clientes
6. Desconexão remove cliente da lista

## Testes Realizados

### Compilação

- ✅ Compilação sem erros
- ✅ Todos os executáveis gerados

### Funcionalidade

- ✅ Servidor inicia na porta 8080
- ✅ Múltiplos clientes conectam simultaneamente
- ✅ Mensagens retransmitidas corretamente
- ✅ Logs gerados com thread safety
- ✅ Script de teste automático funciona

### Logs Gerados

- server.log - Log do servidor com timestamps
- client\_[usuario].log - Log individual de cada cliente

## Métricas

- Linhas de código C++: ~400 linhas
- Arquivos de cabeçalho: 4
- Arquivos de implementação: 5
- Conceitos de concorrência: 5 (threads, mutex, sockets, logging, RAII)

## Conclusão

Etapa 2 completamente implementada e testada, atendendo todos os requisitos obrigatórios com implementação thread-safe.
