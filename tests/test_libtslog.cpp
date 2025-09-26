#include "libtslog/tslog.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

// Função que cada thread vai executar
// Cada thread escreve algumas mensagens de log
void thread_function(int thread_num, tslog::TSLogger* logger) {
    
    // Cada thread escreve 5 mensagens de log
    for (int i = 1; i <= 5; i++) {
        
        // Simula um pouco de trabalho
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Escreve uma mensagem de cada tipo
        logger->debug("Thread " + std::to_string(thread_num) + " - DEBUG message " + std::to_string(i));
        logger->info("Thread " + std::to_string(thread_num) + " - INFO message " + std::to_string(i));
        logger->warning("Thread " + std::to_string(thread_num) + " - WARNING message " + std::to_string(i));
        logger->error("Thread " + std::to_string(thread_num) + " - ERROR message " + std::to_string(i));
    }
    
    // Mensagem final da thread
    logger->info("Thread " + std::to_string(thread_num) + " FINISHED!");
}

int main() {
    std::cout << "=== TESTE DA BIBLIOTECA LIBTSLOG ===" << std::endl;
    std::cout << "Testando logging thread-safe com múltiplas threads..." << std::endl;
    
    // Cria o logger (vai escrever no console e no arquivo "test.log")
    tslog::TSLogger logger("test.log", true);
    
    logger.info("=== INICIO DO TESTE ===");
    
    // Cria 3 threads que vão escrever logs ao mesmo tempo
    std::vector<std::thread> threads;
    
    for (int i = 1; i <= 3; i++) {
        threads.push_back(std::thread(thread_function, i, &logger));
    }
    
    logger.info("3 threads criadas e iniciadas");
    
    // Espera todas as threads terminarem
    for (auto& t : threads) {
        t.join();
    }
    
    logger.info("=== TESTE CONCLUIDO ===");
    
    std::cout << "\nTeste concluído!" << std::endl;
    std::cout << "Verifique o arquivo 'test.log' para ver todos os logs." << std::endl;
    std::cout << "Note que as mensagens das diferentes threads estão misturadas," << std::endl;
    std::cout << "mas cada linha está completa e legível (thread-safe!)." << std::endl;
    
    return 0;
}
