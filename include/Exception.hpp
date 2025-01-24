#pragma once

#include <iostream>
#include <cstring>
#include <exception>
#include <sstream>

class Exception : public std::exception {
    private:
        std::string _message;
        std::string _fileName;
        std::string _functionName;
        int _lineNumber;
        std::string _fullMessage;
        
    public:
        Exception(const std::string& fileName, const std::string& function, int line, const std::string& message)
        : _fileName(fileName), _functionName(function), _lineNumber(line), _message(message) {
            std::ostringstream oss;
            oss << _functionName << " in " << _fileName << " at line " << _lineNumber << ": " << _message;
            _fullMessage = oss.str();
        }
        virtual ~Exception() throw() {}
        virtual const char* what() const throw() {return _fullMessage.c_str();}
};