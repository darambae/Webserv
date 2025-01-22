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

    public:
        // Logger() {};
        // ~Logger() {};
        static std::string error_from;
        static void setErrorLocation(const std::string& location) { error_from = location; };
        static std::string  getTime();
        static void    log(LogType type, const std::string& msg, int errno_set);

};


