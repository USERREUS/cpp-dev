#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <cctype>
#include <cstdlib>

// Функция для обрезания пробелов слева и справа от строки
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    size_t end = s.find_last_not_of(" \t\n\r\f\v");

    if (start != std::string::npos && end != std::string::npos) {
        return s.substr(start, end - start + 1);
    } else {
        return "";
    }
}

// Функция для URL-декодирования строки
std::string urlDecode(const std::string& str) {
    std::string result;
    char hex[3] = {0};

    for (std::size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            hex[0] = str[i + 1];
            hex[1] = str[i + 2];
            result += static_cast<char>(std::strtol(hex, nullptr, 16));
            i += 2;
        } else {
            result += str[i];
        }
    }

    return result;
}

std::unordered_map<std::string, std::string> parseCookies(const std::string& cookies) {
    std::unordered_map<std::string, std::string> cookieMap;
    std::istringstream cookieStream(cookies);

    std::string cookie;
    while (std::getline(cookieStream, cookie, ';')) {
        std::istringstream cookiePairStream(cookie);
        std::string key, value;

        if (std::getline(cookiePairStream, key, '=')) {
            std::getline(cookiePairStream, value);
            key = trim(urlDecode(key));
            value = trim(urlDecode(value));
            cookieMap[key] = value;
        }
    }

    return cookieMap;
}

int main() {
    std::string cookies = "name=%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82; age=30; token=abcdef123456";

    std::unordered_map<std::string, std::string> cookieMap = parseCookies(cookies);

    for (const auto& pair : cookieMap) {
        std::cout << "|" << pair.first << "| = " << pair.second << std::endl;
    }

    return 0;
}
