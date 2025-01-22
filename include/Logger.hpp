#pragma once

#include <iostream>
#include <cstring>
#include <cerrno>
#include <ctime>


enum LogType {
    DEBUG,
    INFO,
    ERROR
};

class Logger {
    private:
        Logger() {};
        Logger(const Logger&);
        Logger& operator=(const Logger&);

        std::string _errorLocation;
        std::string _errorFunction;

    public:
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }
        void    log(LogType type, const char* file, const char* func, const char* msg, int errno_set);
        void    log(LogType type, const char* msg, int errno_set = 0);
        static std::string  getTime();
        std::string getErrorLocation() const { return _errorLocation; }
        std::string getErrorFunction() const { return _errorFunction; }
        void    setErrorLocation(const char* file, const char* func);
};


