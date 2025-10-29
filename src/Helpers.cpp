#include "Helpers.hpp"

std::vector<std::string> Helpers::split(const char* str, char delim) {
                std::vector<std::string> splitted;
                std::stringstream ss(str);
                std::string token;
                while (std::getline(ss, token, delim)) {
                        splitted.push_back(token);
                }
                return splitted;
        }
    
