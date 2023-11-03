#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// Функция для декодирования URL-кодированной строки
std::string rawURLDecode(const std::string& input) {
    std::ostringstream output;
    for (std::string::size_type i = 0; i < input.length(); ++i) {
        if (input[i] == '%') {
            // Если встречается символ '%', декодируем следующие два символа
            if (i + 2 < input.length()) {
                char decoded_char = static_cast<char>(std::stoi(input.substr(i + 1, 2), nullptr, 16));
                output << decoded_char;
                i += 2; // Пропускаем декодированные символы
            } else {
                // Если не хватает символов для декодирования, оставляем '%'
                output << input[i];
            }
        } else if (input[i] == '+') {
            // Заменяем '+' на пробел
            output << ' ';
        } else {
            // Остальные символы остаются без изменений
            output << input[i];
        }
        std::cout << output.str() << std::endl;
    }
    return output.str();
}

int main() {
    std::string encodedString = "%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82%2C%20%D0%BC%D0%B8%D1%80%21";
    std::string decodedString = rawURLDecode(encodedString);
    std::cout << "Декодированная строка: " << decodedString << std::endl;
    return 0;
}


