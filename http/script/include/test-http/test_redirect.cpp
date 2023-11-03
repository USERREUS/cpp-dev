#include <iostream>

int main() {
    // Отправляем заголовок для выполнения редиректа
    std::cout << "Status: 302 Found\n";
    std::cout << "Location: test_gen_html_form.cgi\n\n";

    // Тело ответа (может быть пустым или содержать дополнительную информацию)

    return 0;
}
