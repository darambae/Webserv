#include "../include/Logger.hpp"

Logger::Logger(OutputType type) {
    if (type == FILE_OUTPUT) {
        this->_logFile.open("log.txt", std::ios::app | std::ios::out);
        if (!this->_logFile.is_open())
            THROW("Error opening log file");
    }
    Logger::_output_type = type;
}

Logger::~Logger() {
    if (this->_output_type == FILE_OUTPUT && this->_logFile.is_open())
        this->_logFile.close();
}

Logger  &Logger::getInstance(OutputType type) {
    static Logger instance(type);
    return instance;
}

std::string Logger::getTime() {
    time_t  now = time(0);
    struct tm tstruct;
    char    buf[80];

    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

void    Logger::log(LogType type, const char* msg) {
    std::string log_type;
    if (type == 0)
        log_type = "DEBUG";
    else if (type == 1)
        log_type = "INFO";
    else if (type == 2)
        log_type = "ERROR";
    
    if (type == 3) //LogType::OFF
        return;

    std::string log_msg = "[" + log_type + "] [" + getTime() + "] " + msg + "\n";
    if (_output_type == CONSOLE_OUTPUT)
        std::cout << log_msg;
    else if (_output_type == FILE_OUTPUT) {
        if (_logFile.is_open())
            _logFile << log_msg;
        else
            THROW("Error writing to log file");
    }
}

