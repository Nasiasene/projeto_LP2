#!/bin/bash

# Script para testar o chat com múltiplos clientes
# Uso: ./test_chat.sh [numero_de_clientes]

NUM_CLIENTES=${1:-3}  # 3 clientes por padrão
SERVIDOR_IP="127.0.0.1"
PORTA=8080

echo "=== TESTE DO CHAT MULTIUSUÁRIO ==="
echo "Número de clientes: $NUM_CLIENTES"
echo "Servidor: $SERVIDOR_IP:$PORTA"
echo "================================="

# Compilar projeto
echo "Compilando projeto..."
make clean > /dev/null 2>&1
make all

if [ $? -ne 0 ]; then
    echo "ERRO: Falha na compilação!"
    exit 1
fi

echo "Compilação concluída!"
echo ""

# Função para parar todos os processos
cleanup() {
    echo -e "\nParando todos os processos..."
    kill $(jobs -p) > /dev/null 2>&1
    wait
    echo "Teste finalizado."
    exit 0
}

# Capturar Ctrl+C
trap cleanup SIGINT

# Iniciar servidor em background
echo "Iniciando servidor..."
./build/server $PORTA > server_output.log 2>&1 &
SERVER_PID=$!

# Aguardar servidor inicializar
sleep 2

# Verificar se servidor está rodando
if ! kill -0 $SERVER_PID > /dev/null 2>&1; then
    echo "ERRO: Servidor não conseguiu iniciar!"
    echo "Verifique server_output.log para detalhes."
    exit 1
fi

echo "Servidor iniciado (PID: $SERVER_PID)"
echo ""

# Iniciar clientes
echo "Iniciando $NUM_CLIENTES clientes..."

for i in $(seq 1 $NUM_CLIENTES); do
    USUARIO="Usuario$i"
    echo "Iniciando cliente: $USUARIO"
    
    # Criar script temporário para cada cliente
    cat > /tmp/client_input_$i.txt << EOF
Olá! Sou o $USUARIO
Esta é uma mensagem de teste $i
quit
EOF
    
    # Iniciar cliente em background
    ./build/client $SERVIDOR_IP $PORTA $USUARIO < /tmp/client_input_$i.txt > client_$i.log 2>&1 &
    
    # Pequena pausa entre clientes
    sleep 0.5
done

echo ""
echo "Todos os clientes iniciados!"
echo "Aguardando 10 segundos para troca de mensagens..."
echo ""

# Aguardar um tempo para a troca de mensagens
sleep 10

# Parar tudo
echo "Tempo de teste esgotado. Parando servidor e clientes..."
cleanup
