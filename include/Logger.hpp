#pragma once
#include "webserv.hpp"

enum LogType {
    DEBUG,
    INFO,
    ERROR,
    OFF
};

enum OutputType {
    CONSOLE_OUTPUT,
    FILE_OUTPUT
};

class Logger {

    private:

        std::ofstream   _logFile;
        OutputType _output_type;
        Logger(OutputType type);
        Logger(const Logger&);
        Logger& operator=(const Logger&); 

    public:
    
        static Logger &getInstance(OutputType type);
        void log(LogType type, const std::string& msg, bool errno_set = 0);
        static std::string  getTime();
        ~Logger();
};