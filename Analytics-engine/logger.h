#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>
#include <string>

enum LogLevel { DEBUG, INFO, WARN, LOG_ERROR };

class Logger {
public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void setLevel(LogLevel lvl) { level = lvl; }

    void enableFile(const std::string& path) {
        file.open(path, std::ios::app);
    }

    void log(LogLevel lvl, const std::string& msg) {
        if (lvl < level) return;

        std::lock_guard<std::mutex> lock(m);

        std::string tag = levelTag(lvl);
        std::string ts = timestamp();

        std::string line = "[" + ts + "][" + tag + "] " + msg + "\n";

        std::cout << line;
        if (file.is_open()) file << line;
    }

private:
    std::mutex m;
    std::ofstream file;
    LogLevel level = INFO;

    Logger() {}

    std::string levelTag(LogLevel lvl) {
        switch (lvl) {
            case DEBUG: return "DEBUG";
            case INFO:  return "INFO";
            case WARN:  return "WARN";
            default:    return "ERROR";
        }
    }

    std::string timestamp() {
        std::time_t t = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S",
                      std::localtime(&t));
        return buf;
    }
};
