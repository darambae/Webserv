#pragma once

#include <iostream>
#include <cstring>

enum LogType{
    DEBUG,
    INFO,
    ERROR
};

class Logger {

    public:
        // Logger() {};
        // ~Logger() {};

        const std::string&  getTime();
        static void    log(LogType type, const std::string& location, const std::string& msg, int errno_set);

};


