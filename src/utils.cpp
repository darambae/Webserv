#include "../include/Utils.hpp"

std::string   fullPath(const std::string& name) {
    struct stat buffer;
    char resolved_path[1000]; //possible memory leak
    //LOG_INFO("Given path name: " + name);
    std::string path = name[0] == '/' ? name.substr(1) : name;
    if (realpath(path.c_str(), resolved_path) == NULL)
        THROW("Realpath failed");
    //LOG_INFO("Resolved path : " + std::string(resolved_path));
    if (stat(resolved_path, &buffer) == -1) 
        THROW("Invalid path");
    if (S_ISDIR(buffer.st_mode)) //if its a directory
        ;
    else if (S_ISREG(buffer.st_mode)) //if its a file
        ;
    else
        THROW("Invalid path");
    return resolved_path;
}
bool    onlyDigits(const std::string& str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (!std::isdigit(str[i]))
            return false;
    }
    return true;
}

void    removeWhitespaces(std::string& str) { 
    size_t  start = str.find_first_not_of(" \t");
    size_t  end = str.find_last_not_of(" \t");

    if (start == std::string::npos)
        str = "";
    else
        str = str.substr(start, end - start + 1);
}

bool    endingSemicolon(const std::string& str) {
    return str[str.size() - 1] == ';';
}

//Checking if a string contains a single valid bracket(a) and not contains another bracket(b) in the string
bool    validBracket(const std::string& str, char a, char b) {
    return str.find(a) != std::string::npos && str.find(b) == std::string::npos &&
            str.find_first_of(a) == str.find_last_of(a);
}