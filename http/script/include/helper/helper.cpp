#include "helper.hpp"

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

std::string Helper::getenvHandler(const char* name) { 
    if ( getenv(name) != nullptr )
        return getenv(name);
    
    return ""; 
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

std::string Helper::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    size_t end = s.find_last_not_of(" \t\n\r\f\v");

    if (start != std::string::npos && end != std::string::npos) {
        return s.substr(start, end - start + 1);
    } else {
        return "";
    }
}

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
