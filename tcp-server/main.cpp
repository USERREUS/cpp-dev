#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <map>

// Функция для извлечения значения Content-Length из строки запроса
std::string extractContentLength(const std::string& httpRequest) {
    size_t contentLengthPos = httpRequest.find("Content-Length:");

    if (contentLengthPos != std::string::npos) {
        size_t lineEnd = httpRequest.find("\r\n", contentLengthPos);

        if (lineEnd != std::string::npos) {
            // Находим значение Content-Length между "Content-Length:" и символом новой строки "\r\n"
            size_t valueStart = contentLengthPos + strlen("Content-Length:");
            std::string contentLengthValue = httpRequest.substr(valueStart, lineEnd - valueStart);

            // Удаляем лишние пробелы
            size_t firstNonSpace = contentLengthValue.find_first_not_of(" \t");
            size_t lastNonSpace = contentLengthValue.find_last_not_of(" \t");
            contentLengthValue = contentLengthValue.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);

            return contentLengthValue;
        }
    }

    return "";
}

// Функция для извлечения значения Cookie из строки запроса
std::string extractCookie(const std::string& httpRequest) {
    size_t cookiePos = httpRequest.find("Cookie:");

    if (cookiePos != std::string::npos) {
        size_t lineEnd = httpRequest.find("\r\n", cookiePos);

        if (lineEnd != std::string::npos) {
            // Находим значение Cookie между "Cookie:" и символом новой строки "\r\n"
            size_t valueStart = cookiePos + strlen("Cookie:");
            std::string cookieValue = httpRequest.substr(valueStart, lineEnd - valueStart);

            // Удаляем лишние пробелы
            size_t firstNonSpace = cookieValue.find_first_not_of(" \t");
            size_t lastNonSpace = cookieValue.find_last_not_of(" \t");
            cookieValue = cookieValue.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);

            return cookieValue;
        }
    }

    return "";
}

// Функция для извлечения параметров из строки запроса
std::string extractQueryString(const std::string& data) {
    size_t questionMarkPos = data.find('?');

    if (questionMarkPos != std::string::npos) {
        size_t spacePos = data.find(' ', questionMarkPos);

        if (spacePos != std::string::npos) {
            // Находим позицию пробела после символа '?' и возвращаем часть строки между ними
            return data.substr(questionMarkPos + 1, spacePos - (questionMarkPos + 1));
        }
    }

    // Если символ '?' не найден или пробел не найден после '?', возвращаем пустую строку
    return "";
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    size_t end = s.find_last_not_of(" \t\n\r\f\v");

    if (start != std::string::npos && end != std::string::npos) {
        return s.substr(start, end - start + 1);
    } else {
        return "";
    }
}

// Функция для обработки POST-запроса
void handlePostData(const std::string& postData) {
    // Здесь вы можете обработать данные POST в соответствии с их форматом (например, JSON или форма)
    std::cout << "Данные POST:\n" << postData << std::endl;
}

std::map<std::string, std::string> parseForm(const std::string &input) {
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

// Функция для извлечения значения Content-Type из строки запроса
std::string extractContentType(const std::string& httpRequest) {
    size_t contentTypePos = httpRequest.find("Content-Type:");

    if (contentTypePos != std::string::npos) {
        size_t lineEnd = httpRequest.find("\r\n", contentTypePos);

        if (lineEnd != std::string::npos) {
            // Находим значение Content-Type между "Content-Type:" и символом новой строки "\r\n"
            size_t valueStart = contentTypePos + strlen("Content-Type:");
            std::string contentTypeValue = httpRequest.substr(valueStart, lineEnd - valueStart);
            
            // Удаляем лишние пробелы
            size_t firstNonSpace = contentTypeValue.find_first_not_of(" \t");
            size_t lastNonSpace = contentTypeValue.find_last_not_of(" \t");
            contentTypeValue = contentTypeValue.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);

            return contentTypeValue;
        }
    }

    return "";
}

// Функция для определения HTTP-метода из строки запроса
std::string getHttpMethod(const std::string& request) {
    size_t spacePos = request.find(' ');

    if (spacePos != std::string::npos) {
        return request.substr(0, spacePos);
    }

    return "";
}

void handleClient(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        std::cerr << "Ошибка при чтении данных от клиента" << std::endl;
        close(clientSocket);
        return;
    }

    // Разбиение HTTP-запроса на заголовки
    std::string httpRequest(buffer, bytesRead);

    // Определение HTTP-метода
    std::string httpMethod = getHttpMethod(httpRequest);
    std::cout << "HTTP метод: " << httpMethod << std::endl;

    std::string httpQueryString = extractQueryString(httpRequest);
    std::cout << "HTTP query string: " << httpQueryString << std::endl;

    std::string httpCookie = extractCookie(httpRequest);
    std::cout << "HTTP cookie: " << httpCookie << std::endl;

    std::string httpContentLength = extractContentLength(httpRequest);
    std::cout << "HTTP content-lenght: " << httpContentLength << std::endl;

    // Если метод POST и есть данные, обрабатываем их
    if (httpMethod == "POST") {
        // Определение Content-Type
        std::string contentType = extractContentType(httpRequest);
        std::cout << "Content-Type: " << contentType << std::endl;

        size_t contentLengthPos = httpRequest.find("Content-Length:");
        if (contentLengthPos != std::string::npos) {
            size_t lineEnd = httpRequest.find("\r\n", contentLengthPos);
            if (lineEnd != std::string::npos) {
                size_t valueStart = contentLengthPos + strlen("Content-Length:");
                std::string contentLengthValue = httpRequest.substr(valueStart, lineEnd - valueStart);
                size_t contentLength = std::stoi(contentLengthValue);

                // Читаем данные из тела POST-запроса
                size_t bodyStart = httpRequest.find("\r\n\r\n") + 4;
                std::string postData = httpRequest.substr(bodyStart, contentLength);

                // Обработка данных POST
                handlePostData(postData);
            }
        }
    }

    // Генерация HTML-страницы с заголовками
    std::string htmlResponse = "<html><head><title>HTTP Server</title></head><body>"
                               "<h1>HTTP Server</h1><pre>";

    // Добавление HTTP метода и Query параметров в HTML-страницу
    htmlResponse += "HTTP метод: " + getHttpMethod(httpRequest) + "\n";
    htmlResponse += "</pre></body></html>";

    // Отправка HTML-страницы клиенту
    std::string httpResponse = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: " +
                               std::to_string(htmlResponse.size()) + "\r\n\r\n" + htmlResponse;

    ssize_t bytesSent = send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);

    if (bytesSent == -1) {
        std::cerr << "Ошибка при отправке данных клиенту" << std::endl;
    }

    // Закрытие сокета клиента
    close(clientSocket);
}

int main() {
    // Создание сокета
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return -1;
    }

    // Настройка параметров сервера
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // порт
    serverAddress.sin_addr.s_addr = INADDR_ANY; // принимать соединения от любого адреса

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Ошибка привязки сокета" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Прослушивание порта
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Ошибка прослушивания порта" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Сервер запущен. Ожидание подключений..." << std::endl;

    // Принятие соединений и обработка данных
    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Принятие соединения
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Ошибка при принятии соединения" << std::endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::cout << "Подключен клиент: " << clientIP << std::endl;

        // Обработка данных от клиента и вывод Query параметров
        handleClient(clientSocket);
    }

    // Закрытие серверного сокета
    close(serverSocket);

    return 0;
}
