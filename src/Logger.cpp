#include "../include/Logger.hpp"

std::string Logger::getTime() {
    return "date and time";
}

void    Logger::log(LogType type, const char* file, const char* func, const char* msg, int errno_set = 0) {
    std::string log_type;
    switch (type) {
    case 0: //DEBUG
        log_type = "DEBUG";
        break;
    case 1: //INFO
        log_type = "INFO";
        break;
    case 2: //ERROR
        log_type = "ERROR";
        break;
    default:
        log_type = "UNKNOWN";
        break;
    }

    std::cout << "[" << log_type << "] " << "[" << getTime() << "] ";
    if (errno_set && (type == 2 || type == 0)) {
        std::cout << func << " in " << file << ": " << strerror(errno) << std::endl;
    } else {
        std::cout << msg << std::endl;
    }
}

void    Logger::log(LogType type, const char* msg, int errno_set) {
    log(type, getErrorLocation().c_str(), __FUNCTION__, msg, errno_set);
}


void    Logger::setErrorLocation(const char* file, const char* func) {
    _errorLocation = file;
    _errorFunction = func;
}