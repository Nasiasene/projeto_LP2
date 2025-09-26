#include "libtslog/tslog.h"
#include <sstream>
#include <thread>
#include <chrono>
#include <iomanip>

namespace tslog {

    TSLogger::TSLogger(const std::string& filename, bool console) : use_console_(console) {
        if (!filename.empty()) {
            file_.open(filename, std::ios::app);
            if (!file_.is_open()) {
                std::cerr << "ERRO: Não conseguiu abrir arquivo: " << filename << std::endl;
            }
        }
    }

    TSLogger::~TSLogger() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    void TSLogger::write_log(const std::string& level, const std::string& message) {
        // Timestamp simples
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        // Thread ID
        std::stringstream ss;
        ss << std::this_thread::get_id();
        
        // Monta mensagem: [HH:MM:SS] [LEVEL] [Thread:ID] mensagem
        std::stringstream log_line;
        log_line << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") << "]"
                 << " [" << level << "] [Thread:" << ss.str() << "] " << message;
        
        std::string line = log_line.str();
        
        // Console
        if (use_console_) {
            std::cout << line << std::endl;
        }
        
        // Arquivo
        if (file_.is_open()) {
            file_ << line << std::endl;
            file_.flush();
        }
    }

    void TSLogger::debug(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        write_log("DEBUG", message);
    }

    void TSLogger::info(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        write_log("INFO", message);
    }

    void TSLogger::warning(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        write_log("WARN", message);
    }

    void TSLogger::error(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        write_log("ERROR", message);
    }

} // namespace tslog

