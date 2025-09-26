#ifndef TSLOG_H
#define TSLOG_H

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

namespace tslog {

    // Biblioteca de logging thread-safe simples
    class TSLogger {
    private:
        std::mutex mutex_;        // Exclusão mútua para thread-safety
        std::ofstream file_;      // Arquivo de log
        bool use_console_;        // Se mostra no console
        
        void write_log(const std::string& level, const std::string& message);

    public:
        TSLogger(const std::string& filename = "", bool console = true);
        ~TSLogger();
        
        // Não permite cópia
        TSLogger(const TSLogger&) = delete;
        TSLogger& operator=(const TSLogger&) = delete;
        
        void debug(const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
    };

} // namespace tslog

#endif // TSLOG_H
