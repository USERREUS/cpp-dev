#include "helper.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <filesystem>
#include <map>
#include <random>
#include <chrono>
#include <iomanip>
#include <fstream>

// Функция для логирования
void Helper::log(LogLevel level, const std::string& message) {
    // Получаем текущую дату и время
    auto now = std::chrono::system_clock::now();
    auto timePoint = std::chrono::system_clock::to_time_t(now);

    // Выводим дату-время в формате ГГГГ-ММ-ДД ЧЧ:ММ:СС
    std::cout << std::put_time(std::localtime(&timePoint), "%Y-%m-%d %H:%M:%S");

    // Выводим уровень логирования
    switch (level) {
        case INFO:
            std::cout << " [INFO] ";
            break;
        case DEBUG:
            std::cout << " [DEBUG] ";
            break;
        case ERROR:
            std::cout << " [ERROR] ";
            break;
        default:
            std::cout << " [UNKNOWN] ";
    }

    // Выводим сообщение
    std::cout << message << std::endl;
}

std::string Helper::readFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::in);

    if (file) {
        std::ostringstream contents;
        contents << file.rdbuf();
        file.close();
        return contents.str();
    }

    return "";
}

//FIX!!!
std::string Helper::encodeData(const std::map<std::string, std::string>& data) {
    std::string result;
    for (const auto& pair : data) {
        if (!result.empty()) {
            result += "&";
        }
        //result += urlEncode(pair.first) + "=" + urlEncode(pair.second);
        result += pair.first + "=" + pair.second;
    }
    return result;
}

std::string Helper::urlEncode(const std::string& data) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : data) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else if (c == ' ') {
            escaped << '+';
        } else {
            escaped << '%' << static_cast<int>(static_cast<unsigned char>(c));
        }
    }

    return escaped.str();
}

std::string Helper::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    size_t end = s.find_last_not_of(" \t\n\r\f\v");

    if (start != std::string::npos && end != std::string::npos) {
        return s.substr(start, end - start + 1);
    } else {
        return "";
    }
}

//FIX
std::map<std::string, std::string> Helper::parseForm(const std::string &input) {
    std::map<std::string, std::string> result;
    std::size_t pos = 0;

    while (pos < input.length()) {
        std::size_t equalsPos = input.find('=', pos);
        if (equalsPos == std::string::npos) {
            break; // No '=' found, exit the loop
        }

        std::string key = input.substr(pos, equalsPos - pos);
        pos = equalsPos + 1;

        std::size_t ampersandPos = input.find('&', pos);
        if (ampersandPos == std::string::npos) {
            ampersandPos = input.length();
        }

        std::string value = input.substr(pos, ampersandPos - pos);
        pos = ampersandPos + 1;

        // URL-decode key and value
        //key = trim(urlDecode(key));
        //value = trim(urlDecode(value));

        result[trim(key)] = value;
    }

    return result;
}

//FIX
std::map<std::string, std::string> Helper::parseCookies(const std::string& cookies) {
    std::map<std::string, std::string> cookieMap;
    std::istringstream cookieStream(cookies);

    std::string cookie;
    while (std::getline(cookieStream, cookie, ';')) {
        std::istringstream cookiePairStream(cookie);
        std::string key, value;

        if (std::getline(cookiePairStream, key, '=')) {
            std::getline(cookiePairStream, value);
            //key = trim(urlDecode(key));
            //value = trim(urlDecode(value));
            cookieMap[trim(key)] = value;
        }
    }

    return cookieMap;
}

std::string Helper::generateRandomString(int length) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

    std::string randomString;
    for (int i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }

    return randomString;
}

std::string Helper::urlDecode(const std::string& str) {
    std::string result;
    char hex[3] = {0};

    for (std::size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            hex[0] = str[i + 1];
            hex[1] = str[i + 2];
            result += static_cast<char>(std::stoi(hex, nullptr, 16));
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }

    return result;
}

std::string Helper::extractHttpMethod(const std::string& httpRequest) {
    size_t spacePos = httpRequest.find(' ');

    if (spacePos != std::string::npos) {
        return httpRequest.substr(0, spacePos);
    }

    return "";
}

std::string Helper::extractHeader(const std::string& httpRequest, const std::string& headerName) {
    size_t headerLengthPos = httpRequest.find(headerName + ":");

    if (headerLengthPos != std::string::npos) {
        size_t lineEnd = httpRequest.find("\r\n", headerLengthPos);

        if (lineEnd != std::string::npos) {
            // Находим значение Header между "Header:" и символом новой строки "\r\n"
            size_t valueStart = headerLengthPos + headerName.length() + 1;
            std::string headerLengthValue = httpRequest.substr(valueStart, lineEnd - valueStart);

            // Удаляем лишние пробелы
            size_t firstNonSpace = headerLengthValue.find_first_not_of(" \t");
            size_t lastNonSpace = headerLengthValue.find_last_not_of(" \t");
            headerLengthValue = headerLengthValue.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);

            return headerLengthValue;
        }
    }

    return "";
}

std::string Helper::extractQueryString(const std::string& httpRequest) {
    size_t questionMarkPos = httpRequest.find('?');

    if (questionMarkPos != std::string::npos) {
        size_t spacePos = httpRequest.find(' ', questionMarkPos);

        if (spacePos != std::string::npos) {
            // Находим позицию пробела после символа '?' и возвращаем часть строки между ними
            return httpRequest.substr(questionMarkPos + 1, spacePos - (questionMarkPos + 1));
        }
    }

    // Если символ '?' не найден или пробел не найден после '?', возвращаем пустую строку
    return "";
}

std::string Helper::extractPath(const std::string& httpRequest) {
    size_t pathStartPos = httpRequest.find("/");
    size_t pathEndPos = httpRequest.find(" ", pathStartPos);

    if (pathStartPos != std::string::npos && pathEndPos != std::string::npos) {
        std::string url = httpRequest.substr(pathStartPos, pathEndPos - pathStartPos);
        size_t pathEndPos = url.find("?");
        return (pathEndPos != std::string::npos) ? url.substr(0, pathEndPos) : url;
    }

    return "";
}
