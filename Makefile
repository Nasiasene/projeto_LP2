# Projeto LP2 - Chat Multiusuário TCP
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -I./include

# Diretórios
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

# Arquivos fonte
LIBTSLOG_SRC = $(SRC_DIR)/libtslog/tslog.cpp
TEST_SRC = $(TEST_DIR)/test_libtslog.cpp

# Executáveis
TEST_EXEC = $(BUILD_DIR)/test_libtslog

# Compilar
all: $(TEST_EXEC)

# Teste da biblioteca libtslog
$(TEST_EXEC): $(LIBTSLOG_SRC) $(TEST_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIBTSLOG_SRC) $(TEST_SRC) -o $(TEST_EXEC)
	@echo "Biblioteca libtslog compilada com sucesso!"

# Executar teste
test: $(TEST_EXEC)
	@echo "=== Testando libtslog ==="
	./$(TEST_EXEC)

# Limpar
clean:
	rm -rf $(BUILD_DIR)
	rm -f *.log

.PHONY: all test clean
