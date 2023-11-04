#ifndef HELPER_HPP
#define HELPER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <map>

class Helper {
public:
    static std::string encodeData(const std::map<std::string, std::string>& data);
    static std::string urlEncode(const std::string& data);
    static std::string getenvHandler(const char* name);
    static std::string trim(const std::string& s);
    static std::map<std::string, std::string> parseForm(const std::string &data);
    static std::map<std::string, std::string> parseCookies(const std::string& cookies);
    static std::string urlDecode(const std::string& str);
};

#endif