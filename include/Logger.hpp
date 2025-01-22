#pragma once

#include <iostream>
#include <cstring>

enum    LogType {
    DEBUG,
    INFO,
    ERROR
}

class   Logger  {
    private:
        
    public:
        Logger(/* args */);
        ~Logger();

        const std::string&  getTime();
        void    log(LogType type, const std::string& location, const std::string& msg, int errno_set);

};


