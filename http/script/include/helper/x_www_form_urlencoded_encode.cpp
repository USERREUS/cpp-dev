#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <sstream>

std::string urlEncode(const std::string& data) {
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
        std::cout << c << std::endl;
        std::cout << escaped.str() << std::endl;
    }

    return escaped.str();
}

std::string encodeData(const std::unordered_map<std::string, std::string>& data) {
    std::string result;
    for (const auto& pair : data) {
        if (!result.empty()) {
            result += "&";
        }
        result += urlEncode(pair.first) + "=" + urlEncode(pair.second);
    }
    return result;
}

int main() {
    std::unordered_map<std::string, std::string> data = {
        {"name", "Привет, мир!"},
        {"age", "25"}
    };

    std::string encodedData = encodeData(data);

    std::cout << "Закодированные данные: " << encodedData << std::endl;

    return 0;
}
