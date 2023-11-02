#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

int main() {
    // Получаем данные из строки запроса (метод GET)
    char* query_string = std::getenv("QUERY_STRING");
    std::string data;

    if (query_string) {
        char* token = std::strtok(query_string, "&");
        std::unordered_map<std::string, std::string> queryParams;

        while (token != nullptr) {
            char* equals = std::strchr(token, '=');
            if (equals != nullptr) {
                *equals = '\0';
                std::string key = token;
                std::string value = equals + 1;
                queryParams[key] = value;
            }
            token = std::strtok(nullptr, "&");
        }
        std::cout << "Content-type: text/html\n";
        for (auto i : queryParams) {
            std::cout << "Set-Cookie: " << i.first << "=" << i.second << ";\n";
        }
        std::cout << "\n\n";
        std::cout << "Cookies set\n";
    }

    return 0;
}
