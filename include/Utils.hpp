#pragma once

#include "webserv.hpp"

template <typename T>
std::string to_string(T value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}