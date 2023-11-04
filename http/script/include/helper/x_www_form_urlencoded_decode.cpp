#include <iostream>
#include <string>
#include <unordered_map>

std::string urlDecode(const std::string& str) {
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

std::unordered_map<std::string, std::string> parseUrlEncodedString(const std::string& input) {
    std::unordered_map<std::string, std::string> result;
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
        key = urlDecode(key);
        value = urlDecode(value);

        result[key] = value;
    }

    return result;
}

int main() {
    std::string encodedString = "age=25&name=%d0%9f%d1%80%d0%b8%d0%b2%d0%b5%d1%82%2c+%d0%bc%d0%b8%d1%80%21";
    std::unordered_map<std::string, std::string> parsedData = parseUrlEncodedString(encodedString);

    for (const auto& pair : parsedData) {
        std::cout << pair.first << " = " << pair.second << std::endl;
    }

    return 0;
}
