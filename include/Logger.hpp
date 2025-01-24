#pragma once

#include <iostream>
#include <cstring>
#include <cerrno>
#include <ctime>


enum LogType {
    DEBUG,
    INFO,
    ERROR,
    OFF
};

class Logger {
    private:
        Logger() {};
        Logger(const Logger&);
        Logger& operator=(const Logger&); 

    public:
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }
        void log(LogType type, const char *msg);
        static std::string  getTime();
};


