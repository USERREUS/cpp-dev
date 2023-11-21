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
#include <hiredis/hiredis.h>

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

class RedisClient {
    redisContext* context_;

public:
    RedisClient(const std::string& host = "127.0.0.1", int port = 6379) {
        context_ = redisConnect(host.c_str(), port);
        if (context_ == nullptr || context_->err) {
            if (context_) {
                std::cerr << "Ошибка подключения к Redis: " << context_->errstr << std::endl;
            } else {
                std::cerr << "Не удалось выделить память для redisContext" << std::endl;
            }
            exit(1);
        }
    }
    ~RedisClient() {
        if (context_) {
            redisFree(context_);
        }
    }
    void Set(const std::string& key, const std::string& value) {
        redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %s", key.c_str(), value.c_str());
        freeReplyObject(reply);
    }
    std::string Get(const std::string& key) {
        redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
        std::string value;
        if (reply->type == REDIS_REPLY_STRING) {
            value = reply->str;
        }
        freeReplyObject(reply);
        return value;
    }
    std::vector<std::string> GetAllKeys() {
        redisReply* reply = (redisReply*)redisCommand(context_, "KEYS *");
        std::vector<std::string> keys;
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; ++i) {
                keys.push_back(reply->element[i]->str);
            }
        }
        freeReplyObject(reply);
        return keys;
    }
    void Delete(const std::string& key) {
        redisReply* reply = (redisReply*)redisCommand(context_, "DEL %s", key.c_str());
        freeReplyObject(reply);
    }
};

class Session {
    public:
    Session() {
        UUID = generateUUID();
    }
    std::string getUUID() {
        return UUID;
    }
    void set(std::string name, std::string value) {
        client.Set(UUID + name, value);
    }
    std::string get(std::string name) {
        return client.Get(UUID + name);
    }

    private:
    const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghij";

    std::string UUID;
    RedisClient client;
    
    std::map<std::string, std::string> data;

    std::string generateUUID() {
        std::random_device rd; // Инициализируем генератор случайных чисел
        std::mt19937 gen(rd()); // Используем генератор Mersenne Twister
        std::uniform_int_distribution<int> distribution(1, 100); // Задаем диапазон случайных чисел
        std::string uuid = std::string(36,' ');
        int rnd = 0;
        int r = 0;

        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';
        
        uuid[14] = '4';

        for(int i=0;i<36;i++) {
            if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
                if (rnd <= 0x02) {
                    rnd = 0x2000000 + (distribution(gen) * 0x1000000) | 0;
                }
            rnd >>= 4;
            uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
            }
        }

        return uuid;
    }
};

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

class Store {
    std::string name;
    std::map<std::string, std::map<std::string, std::string>> data;

    std::string getNextId() {
        int max_id = 0;
        for (auto i : data) {
            int id = stoi(i.first);
            if (id > max_id) {
                max_id = id;
            }
        }
        return std::to_string(max_id + 1);
    }
    std::string linePretty(std::string id, std::map<std::string, std::string> dict) {
        std::string res = "ID : " + id + "; ";
        for (auto i : dict) {
            res += Helper::urlDecode(i.first) + " : " + Helper::urlDecode(i.second) + "; ";
        }
        return res;
    }
    std::map<std::string, std::map<std::string, std::string>> parse(std::ifstream& in) {
        std::map<std::string, std::map<std::string, std::string>> dict;

        std::string line;
        while (std::getline(in, line)) {
            auto temp = Helper::parseForm(line);
            std::string ID = temp["id"];
            temp.erase("id");
            dict[ID] = temp;
        }

        return dict;
    }

public:
    Store() {
        name = "store.txt";
        std::ifstream file(name);
        data = parse(file);
        file.close();
    }
    Store(std::string fileName) {
        name = fileName;
        std::ifstream file(name);
        data = parse(file);
        file.close();
    }
    std::string AppendData(std::string record) {
        std::string id = getNextId();
        data[id] = Helper::parseForm(record);
        return "Success AppendData";
    }
    std::string AppendData(std::map<std::string, std::string> dict) {
        std::string id = getNextId();
        std::string record = Helper::encodeData(dict); // without urlEncode
        data[id] = Helper::parseForm(record);
        return "Success AppendData";
    }
    std::string DeleteOne(std::string ID) {
        if (data.find(ID) == data.end()) {
            return "Delete Error. ID not found.";
        }
        data.erase(ID);
        return "Success";
    }
    std::string GetOne(std::string ID) {
        if (data.find(ID) == data.end()) {
            return "GetOne error : ID not found";
        }
        return linePretty(ID, data[ID]);
    }
    std::string GetAll() {
        if (data.size() > 0) {
            std::string res;
            for (auto i : data) {
                res += linePretty(i.first, i.second) + "</p>";
            }
            return res;
        }
        return "Store is empty";
    }
    bool emailValidation(std::string email) {
        for (auto rec : data) {
            if (rec.second.find("email") != rec.second.end()) {
                if (rec.second["email"] == email) {
                    return false;
                }
            }
        }
        return true;
    }
    bool dataValidation(std::string email, std::string pass) {
        for (auto rec : data) {
            if (rec.second.find("email") != rec.second.end() && rec.second.find("password") != rec.second.end()) {
                if (rec.second["email"] == email && rec.second["password"] == pass) {
                    return true;
                }
            }
        }
        return false;
    }
    ~Store() {
        std::ofstream file(name);
        for (auto i : data) {
            file << "id=" << i.first << "&" << Helper::encodeData(data[i.first]) << std::endl;
        }
        file.close();
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

bool checkSession(const std::string& httpRequest) {
    std::string cookie = Helper::extractHeader(httpRequest, headers[COOKIE]);
    auto parsed_cookie = Helper::parseCookies(cookie);
    if (parsed_cookie.find("session") != parsed_cookie.end()) {
        Session session;
        log(WARNING, session.get(parsed_cookie["session"] + "name"));
        if (session.get(parsed_cookie["session"] + "name") != "") {
            return true;
        }
    }
    return false;
}

// Функция для обработки POST-запроса
std::string handlePostData(const std::string& httpRequest) {
    // Определение Content-Type
    std::string contentType = Helper::extractHeader(httpRequest, headers[CONTENT_TYPE]);
    std::string httpContentLength = Helper::extractHeader(httpRequest, headers[CONTENT_LENGTH]);
    size_t contentLength = std::stoi(httpContentLength);

    // Читаем данные из тела POST-запроса
    size_t bodyStart = httpRequest.find("\r\n\r\n") + 4;
    std::string postData = httpRequest.substr(bodyStart, contentLength);

    return postData;
}

std::string getNotFoundResponse() { 
    std::string htmlResponse = readFile("notfound.html");
    std::string httpResponse = "HTTP/1.0 404 Not Found\r\n";
    httpResponse += "Content-Type: text/html\r\n";
    httpResponse += "Content-Length: " + std::to_string(htmlResponse.size()) + "\r\n\r\n";
    httpResponse += htmlResponse;
    return httpResponse;
}

std::string getOkResponse(const std::string& htmlResponse) {
    std::string httpResponse = "HTTP/1.0 200 OK\r\n";
    httpResponse += "Content-Type: text/html\r\n";
    httpResponse += "Content-Length: " + std::to_string(htmlResponse.size()) + "\r\n\r\n";
    httpResponse += htmlResponse;
    return httpResponse;
}

std::string getResponse(const std::string& fileName) {
    std::string httpResponse, htmlResponse;
    htmlResponse = readFile(fileName);
    if (!htmlResponse.empty()) {
        httpResponse = getOkResponse(htmlResponse);
    } else {
        httpResponse = getNotFoundResponse();
    }
    return httpResponse;
}

std::string getHomePage(const std::string& name) {
    std::string htmlContent = "<!DOCTYPE html>\n"
                              "<html lang=\"en\">\n"
                              "<head>\n"
                              "    <meta charset=\"UTF-8\">\n"
                              "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                              "    <title>Home</title>\n"
                              "</head>\n"
                              "<body>\n"
                              "    <h1>Welcome to the Home Page!</h1>\n"
                              "    <p>Hello, " + name + "! This is your home page.</p>\n"
                              "</body>\n"
                              "</html>\n";

    std::string response = "HTTP/1.0 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(htmlContent.size()) + "\r\n";
    response += "\r\n" + htmlContent;

    return response;
}

bool signinDataValidation(const std::string& httpRequest) {
    std::string postData = handlePostData(httpRequest);
    auto auth_params = Helper::parseForm(postData);
    if (auth_params.find("email") != auth_params.end() && auth_params.find("password") != auth_params.end()) {
        Store store = Store();
        return store.dataValidation(auth_params["email"], auth_params["password"]);
    }
    return false;
}

std::string homeHandler(const std::string& httpRequest) {
    std::string httpResponse, httpMethod;
    httpMethod = Helper::extractHttpMethod(httpRequest);
    if (httpMethod == "GET") {
        if (checkSession(httpRequest)) {
            log(INFO, "Check Seccess");
            std::string cookie = Helper::extractHeader(httpRequest, headers[COOKIE]);
            auto parsed_cookie = Helper::parseCookies(cookie);
            Session session;
            std::string name = session.get(parsed_cookie["session"] + "name");
            httpResponse = getHomePage(name);
        } else {
            httpResponse = getNotFoundResponse();
        }
    } else {
        httpResponse = getNotFoundResponse();
    }
    
    return httpResponse;
}

std::string signinHandler(const std::string& httpRequest) {
    std::string httpResponse, httpMethod;
    httpMethod = Helper::extractHttpMethod(httpRequest);
    if (httpMethod == "GET") {
        httpResponse = getResponse("signin.html");
    } else if (httpMethod == "POST") {
        if (!signinDataValidation(httpRequest)) {
            log(ERROR, "Incorrect params");
            httpResponse = getResponse("signin.html");
        } else {
            Session session;
            session.set("name", "TestUset");
            httpResponse = "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/home\r\nSet-Cookie: session=" + session.getUUID() + "\r\n\r\n";
        }
    } else {
        httpResponse = getNotFoundResponse();
    }
    
    return httpResponse;
}

bool signupDataValidation(const std::string& httpRequest) {
    Store store = Store();
    std::string postData = handlePostData(httpRequest);
    auto params = Helper::parseForm(postData);
    if (params.find("email") == params.end() || !store.emailValidation(params["email"])) {
        return false;
    }
    store.AppendData(postData);
    return true;
}

std::string signupHandler(const std::string& httpRequest) {
    std::string httpResponse, httpMethod;
    httpMethod = Helper::extractHttpMethod(httpRequest);
    if (httpMethod == "GET") {
        httpResponse = getResponse("signup.html");
    } else if (httpMethod == "POST") {
        if (signupDataValidation(httpRequest)) {
            log(INFO, "Redirect");
            httpResponse = "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/signin\r\n\r\n";
        } else {
            log(ERROR, "Incorrect params");
            httpResponse = getResponse("signup.html");
        }
    } else {
        httpResponse = getNotFoundResponse();
    }
    
    return httpResponse;
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

    std::string httpResponse;

    if (httpPath == "/signup") {
        httpResponse = signupHandler(httpRequest);
    } else if (httpPath == "/signin") {
        httpResponse = signinHandler(httpRequest);
    } else if (httpPath == "/home") {
        httpResponse = homeHandler(httpRequest);
    } else {
        httpResponse = getNotFoundResponse();
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
