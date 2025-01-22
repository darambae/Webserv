#include "../include/Logger.hpp"

std::string Logger::error_from = ""; //Initializing a static variable

std::string Logger::getTime() {
    return "date and time";
}

void    Logger::log(LogType type, const std::string& msg, int errno_set = 0) {
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

    std::cout << "[" << log_type << "] " << "[" << getTime() << "] " << error_from << " : ";
    if (errno_set && type == 2) {
        std::cout << strerror(errno) << std::endl;
    } else {
        std::cout << msg << std::endl;
    }
}
