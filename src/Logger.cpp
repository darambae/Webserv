#include "../include/Logger.hpp"
#include <cerrno>

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
    static Logger consoleLogger(CONSOLE_OUTPUT);
    static Logger fileLogger(FILE_OUTPUT);

    return (type == FILE_OUTPUT) ? fileLogger : consoleLogger;
}

std::string Logger::getTime() {
    time_t  now = time(0);
    struct tm tstruct;
    char    buf[80];

    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

void    Logger::log(LogType type, const std::string& msg, bool errno_set) {
    std::string log_type;
    std::string log_color;
    switch (type) {
        case DEBUG:
            log_type = "DEBUG"; log_color = "\x1B[94m";
            break;
        case INFO:
            log_type = "INFO"; log_color = "\x1B[97m";
            break;
        case ERROR:
            log_type = "ERROR"; log_color = "\x1B[93m";
            break;
        default:
            return; // if it's LOG OFF, don't print msg anywhere.
    }

    int save_errno = errno;
    std::string log_msg = "[" + getTime() + "] [" + log_type + "] " + msg;
    if (log_type == "ERROR" && errno_set)
        log_msg += " : " + std::string(strerror(save_errno));
    log_msg += "\n";
    if (_output_type == CONSOLE_OUTPUT)
        std::cout << log_color << log_msg << "\x1B[0m";
    else if (_output_type == FILE_OUTPUT) {
        if (_logFile.is_open())
            _logFile << log_msg;
        else
            THROW("Error writing to log file");
    }
}

