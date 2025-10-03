# Projeto LP2 - Chat Multiusuário TCP
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -I./include

# Diretórios
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

# Arquivos fonte
LIBTSLOG_SRC = $(SRC_DIR)/libtslog/tslog.cpp
CHAT_SERVER_SRC = $(SRC_DIR)/chat_server.cpp
CHAT_CLIENT_SRC = $(SRC_DIR)/chat_client.cpp
SERVER_MAIN_SRC = $(SRC_DIR)/server_main.cpp
CLIENT_MAIN_SRC = $(SRC_DIR)/client_main.cpp
TEST_SRC = $(TEST_DIR)/test_libtslog.cpp

# Executáveis
TEST_EXEC = $(BUILD_DIR)/test_libtslog
SERVER_EXEC = $(BUILD_DIR)/server
CLIENT_EXEC = $(BUILD_DIR)/client

# Compilar tudo
all: $(TEST_EXEC) $(SERVER_EXEC) $(CLIENT_EXEC)

# Teste da biblioteca libtslog
$(TEST_EXEC): $(LIBTSLOG_SRC) $(TEST_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIBTSLOG_SRC) $(TEST_SRC) -o $(TEST_EXEC)
	@echo "Teste libtslog compilado!"

# Servidor de chat
$(SERVER_EXEC): $(LIBTSLOG_SRC) $(CHAT_SERVER_SRC) $(SERVER_MAIN_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIBTSLOG_SRC) $(CHAT_SERVER_SRC) $(SERVER_MAIN_SRC) -o $(SERVER_EXEC)
	@echo "Servidor compilado!"

# Cliente de chat
$(CLIENT_EXEC): $(LIBTSLOG_SRC) $(CHAT_CLIENT_SRC) $(CLIENT_MAIN_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIBTSLOG_SRC) $(CHAT_CLIENT_SRC) $(CLIENT_MAIN_SRC) -o $(CLIENT_EXEC)
	@echo "Cliente compilado!"

# Executar teste libtslog
test: $(TEST_EXEC)
	@echo "=== Testando libtslog ==="
	./$(TEST_EXEC)

# Executar servidor
server: $(SERVER_EXEC)
	@echo "=== Iniciando servidor ==="
	./$(SERVER_EXEC)

# Executar cliente
client: $(CLIENT_EXEC)
	@echo "=== Iniciando cliente ==="
	./$(CLIENT_EXEC)

# Limpar
clean:
	rm -rf $(BUILD_DIR)
	rm -f *.log

.PHONY: all test server client clean
