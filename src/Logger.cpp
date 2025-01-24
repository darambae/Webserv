#include "../include/Logger.hpp"

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

    std::cout << "[" << log_type << "] " << "[" << getTime() << "] ";
    std::cout << msg << std::endl;
}

