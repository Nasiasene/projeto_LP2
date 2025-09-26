# Projeto LP2 - Chat Multiusuário TCP

**Tema:** Servidor de Chat Multiusuário (TCP)  
**Etapa:** 1 - Biblioteca libtslog + Arquitetura

## Biblioteca libtslog

Biblioteca simples de logging thread-safe. Múltiplas threads podem escrever logs ao mesmo tempo usando `std::mutex`.

### Arquivos:

- `include/libtslog/tslog.h` - Interface
- `src/libtslog/tslog.cpp` - Implementação

### Como usar:

```cpp
tslog::TSLogger logger("arquivo.log", true);  // arquivo + console
logger.info("Mensagem de informação");
logger.error("Mensagem de erro");
```

### Teste:

```bash
make test  # 3 threads escrevendo logs simultaneamente
```

## Arquitetura (Headers para próximas etapas)

- `chat/types.h` - Estruturas básicas (Message, Client)
- `chat/server.h` - Servidor TCP simples
- `chat/client.h` - Cliente CLI simples

## Compilar

```bash
make       # Compila tudo
make test  # Executa teste
make clean # Limpa arquivos
```

## Conceitos Usados

- **Exclusão mútua**: `std::mutex`
- **Threads**: `std::thread`
- **RAII**: Gerenciamento automático de recursos

## Estrutura do Projeto

```
projeto_lp2/
├── include/
│   ├── libtslog/tslog.h
│   └── chat/{types,server,client}.h
├── src/libtslog/tslog.cpp
├── tests/test_libtslog.cpp
├── docs/{arquitetura,analise_critica_ia}.md
└── Makefile
```
