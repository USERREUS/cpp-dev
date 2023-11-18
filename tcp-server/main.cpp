#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <map>
#include <random>
#include <chrono>
#include <iomanip>
#include <fstream>

enum LogLevel {
    INFO,
    WARNING,
    ERROR
};

enum Headers {
    CONTENT_LENGTH = 0,
    COOKIE = 1,
    CONTENT_TYPE = 2
};

const std::vector<std::string> headers = {
    "Content-Length",
    "Cookie",
    "Content-Type"
};

// Функция для логирования
void log(LogLevel level, const std::string& message) {
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
        case WARNING:
            std::cout << " [WARNING] ";
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

class Helper {
public:
    static std::string encodeData(const std::map<std::string, std::string>& data) {
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
    static std::string urlEncode(const std::string& data) {
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
    static std::string getenvHandler(const char* name) {
        if ( getenv(name) != nullptr )
            return getenv(name);
        return ""; 
    }
    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r\f\v");
        size_t end = s.find_last_not_of(" \t\n\r\f\v");

        if (start != std::string::npos && end != std::string::npos) {
            return s.substr(start, end - start + 1);
        } else {
            return "";
        }
    }
    static std::map<std::string, std::string> parseForm(const std::string &input) {
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
    static std::map<std::string, std::string> parseCookies(const std::string& cookies) {
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
    static std::string generateRandomString(int length) {
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
    static std::string urlDecode(const std::string& str) {
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

    static std::string extractHttpMethod(const std::string& httpRequest) {
        size_t spacePos = httpRequest.find(' ');

        if (spacePos != std::string::npos) {
            return httpRequest.substr(0, spacePos);
        }

        return "";
    }
    static std::string extractHeader(const std::string& httpRequest, const std::string& headerName) {
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
    static std::string extractQueryString(const std::string& httpRequest) {
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
    static std::string extractPath(const std::string& httpRequest) {
        size_t pathStartPos = httpRequest.find("/");
        size_t pathEndPos = httpRequest.find(" ", pathStartPos);

        if (pathStartPos != std::string::npos && pathEndPos != std::string::npos) {
            std::string url = httpRequest.substr(pathStartPos, pathEndPos - pathStartPos);
            size_t pathEndPos = url.find("?");
            return (pathEndPos != std::string::npos) ? url.substr(0, pathEndPos) : url;
        }

        return "";
    }
};

// Функция для чтения содержимого файла
std::string readFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::in);

    if (file) {
        std::ostringstream contents;
        contents << file.rdbuf();
        file.close();
        return contents.str();
    }

    return "";
}

// Функция для обработки POST-запроса
void handlePostData(const std::string& postData) {
    log(INFO, "HTTP Данные POST: " + postData);
}

// Обработка данных от клиента
void handleClient(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        log(ERROR, "Ошибка при чтении данных от клиента");
        close(clientSocket);
        return;
    }

    // Разбиение HTTP-запроса на заголовки
    std::string httpRequest(buffer, bytesRead);

    std::string httpMethod = Helper::extractHttpMethod(httpRequest);
    log(INFO, "HTTP метод: " + httpMethod);
    
    std::string httpPath = Helper::extractPath(httpRequest);
    log(INFO, "HTTP path: " + httpPath);

    std::string httpQueryString = Helper::extractQueryString(httpRequest);
    log(INFO, "HTTP query string: " + httpQueryString);

    std::string httpCookie = Helper::extractHeader(httpRequest, headers[COOKIE]);
    log(INFO, "HTTP cookie: " + httpCookie);

    std::string httpContentLength = Helper::extractHeader(httpRequest, headers[CONTENT_LENGTH]);
    log(INFO, "HTTP content-lenght: " + httpContentLength);

    // Если метод POST и есть данные, обрабатываем их
    if (httpMethod == "POST") {
        // Определение Content-Type
        std::string contentType = Helper::extractHeader(httpRequest, headers[CONTENT_TYPE]);
        log(INFO, "HTTP Content-Type: " + contentType);

        size_t contentLength = std::stoi(httpContentLength);

        // Читаем данные из тела POST-запроса
        size_t bodyStart = httpRequest.find("\r\n\r\n") + 4;
        std::string postData = httpRequest.substr(bodyStart, contentLength);

        // Обработка данных POST
        handlePostData(postData);
    }

    // Генерация HTML-страницы с заголовками
    //std::string htmlResponse = "<html><head><title>HTTP Server</title></head><body>"
    //                           "<h1>HTTP Server</h1></body></html>";

    // Отправка HTML-страницы клиенту
    //std::string httpResponse = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: " +
    //                         std::to_string(htmlResponse.size()) + "\r\n\r\n" + htmlResponse;

    std::string httpResponse;

    if (httpPath == "/login") {
        std::string htmlResponse = readFile("login.html");

        if (!htmlResponse.empty()) {
            httpResponse = "HTTP/1.0 200 OK\r\n";
            httpResponse += "Content-Type: text/html\r\n";
            httpResponse += "Content-Length: " + std::to_string(htmlResponse.size()) + "\r\n\r\n";
            httpResponse += htmlResponse;
        } else {
            htmlResponse = readFile("notfound.html");
            httpResponse = "HTTP/1.0 404 Not Found\r\n";
            httpResponse += "Content-Type: text/html\r\n";
            httpResponse += "Content-Length: " + std::to_string(htmlResponse.size()) + "\r\n\r\n";
            httpResponse += htmlResponse;
        }
    } else {
        std::string htmlResponse = readFile("notfound.html");
        httpResponse = "HTTP/1.0 404 Not Found\r\n";
        httpResponse += "Content-Type: text/html\r\n";
        httpResponse += "Content-Length: " + std::to_string(htmlResponse.size()) + "\r\n\r\n";
        httpResponse += htmlResponse;
    }

    ssize_t bytesSent = send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);

    if (bytesSent == -1) {
        log(ERROR, "Ошибка при отправке данных клиенту");
    }

    // Закрытие сокета клиента
    close(clientSocket);
}

int main() {
    // Создание сокета
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        log(ERROR, "Ошибка создания сокета");
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
        log(ERROR, "Ошибка привязки сокета");
        close(serverSocket);
        return -1;
    }

    // Прослушивание порта
    if (listen(serverSocket, 10) == -1) {
        log(ERROR, "Ошибка прослушивания порта");
        close(serverSocket);
        return -1;
    }

    log(INFO, "Сервер запущен. Ожидание подключений...");

    // Принятие соединений и обработка данных
    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Принятие соединения
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            log(ERROR, "Ошибка при принятии соединения");
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::string ipAddress(clientIP);
        log(INFO, "Подключен клиент: " +  ipAddress);

        // Обработка данных от клиента
        handleClient(clientSocket);
    }

    // Закрытие серверного сокета
    close(serverSocket);

    return 0;
}
