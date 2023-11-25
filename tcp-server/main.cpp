#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <filesystem>
#include <map>
#include <random>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <hiredis/hiredis.h>
#include "script/helper/helper.hpp"
#include "script/store/store.hpp"
#include "script/session/session.hpp"
#include "script/http/http.hpp"

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
        Session session(parsed_cookie["session"]);
        if (session.get("name") != "") {
            return true;
        }
    }
    return false;
}

std::string getNotFoundResponse() { 
    std::string htmlResponse = readFile("hmtl/notfound.html");
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

std::string rootHandler(const std::string& httpRequest) {
    std::string httpResponse, httpMethod;
    httpMethod = Helper::extractHttpMethod(httpRequest);

    if (checkSession(httpRequest)) {
        httpResponse = getResponse("html/rootauthorized.html");
    } else {
        httpResponse = getResponse("html/rootunauthorized.html");
    }
    
    return httpResponse;
}

std::string signinHandler(const std::string& httpRequest) {
    std::string httpResponse, httpMethod;
    httpMethod = Helper::extractHttpMethod(httpRequest);
    if (httpMethod == "GET") {
        httpResponse = getResponse("html/signin.html");
    } else if (httpMethod == "POST") {
        if (!signinDataValidation(httpRequest)) {
            log(ERROR, "Incorrect params");
            httpResponse = getResponse("html/signin.html");
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

void makeUserDir(const std::string& name) {
    if (mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0) {
        log(INFO, "Directory created: " + name);
    } else {
        log(ERROR, "Error creating directory");
    }
}

bool signupDataValidation(const std::string& httpRequest) {
    Store store = Store();
    std::string postData = handlePostData(httpRequest);
    auto params = Helper::parseForm(postData);
    if (params.find("email") == params.end() || !store.emailValidation(params["email"])) {
        return false;
    }
    makeUserDir(params["email"]);
    store.AppendData(postData);
    return true;
}

std::string signupHandler(const std::string& httpRequest) {
    std::string httpResponse, httpMethod;
    httpMethod = Helper::extractHttpMethod(httpRequest);
    if (httpMethod == "GET") {
        httpResponse = getResponse("html/signup.html");
    } else if (httpMethod == "POST") {
        if (signupDataValidation(httpRequest)) {
            log(INFO, "Redirect");
            httpResponse = "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/signin\r\n\r\n";
        } else {
            log(ERROR, "Incorrect params");
            httpResponse = getResponse("html/signup.html");
        }
    } else {
        httpResponse = getNotFoundResponse();
    }
    
    return httpResponse;
}

std::string signoutHandler(const std::string& httpRequest) {
    return "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/\r\nSet-Cookie: session=00000000-0000-0000-0000-000000000000\r\n\r\n";
}

std::string generateFileList(const std::string& directoryPath) {
    std::stringstream html;

    html << "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "    <title>File List</title>\n"
            "</head>\n"
            "<body>\n"
            "    <a href=\"/signout\"><button>Sign Out</button></a>\n"
            "<form method=\"post\" enctype=\"multipart/form-data\">\n"
            "    <label for=\"file\">Select File:</label>\n"
            "    <input type=\"file\" name=\"file\" id=\"file\" required>\n"
            "    <br>\n"
            "    <input type=\"submit\" value=\"Upload\">\n"
            "</form>\n"
            "    <h2>File List</h2>\n"
            "    <ul>\n";

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();
            html << "        <li>" << fileName << " - "
                 << "<a href=\"/download?file=" << fileName << "\">Download</a> | "
                 << "<a href=\"/delete?file=" << fileName << "\">Delete</a>"
                 << "</li>\n";
        }
    }

    html << "    </ul>\n"
            "</body>\n"
            "</html>\n";

    return html.str();
}

std::string homeHandler(const std::string& httpRequest) {
    std::string httpResponse, httpMethod;
    httpMethod = Helper::extractHttpMethod(httpRequest);
    if (httpMethod == "GET") {
        if (checkSession(httpRequest)) {
            std::string cookie = Helper::extractHeader(httpRequest, headers[COOKIE]);
            auto parsed_cookie = Helper::parseCookies(cookie);
            Session session(parsed_cookie["session"]);
            std::string name = session.get("name");
            //httpResponse = getHomePage(name);
            httpResponse = getOkResponse(generateFileList("sergsysoev%40gmail.com"));
        } else {
            httpResponse = getNotFoundResponse();
        }
    } else if (httpMethod == "POST") {
        if (checkSession(httpRequest)) {
            HTTP http(httpRequest);
            httpResponse = getOkResponse(generateFileList("sergsysoev%40gmail.com"));
        } else {
            httpResponse = getNotFoundResponse();
        }
    } else {
        httpResponse = getNotFoundResponse();
    }
    
    return httpResponse;
}

std::string sendFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        log(ERROR, "Error opening file: " + filePath);
        return getNotFoundResponse();
    }

    std::ostringstream fileContent;
    fileContent << file.rdbuf();
    file.close();

    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: application/octet-stream\r\n";
    response += "Content-Disposition: attachment; filename=\"" + std::filesystem::path(filePath).filename().string() + "\"\r\n";
    response += "Content-Length: " + std::to_string(fileContent.str().size()) + "\r\n";
    response += "\r\n" + fileContent.str();

    return response;
}

void deleteFile(const std::string& filePath) {
    if (remove(filePath.c_str()) != 0) {
        log(ERROR, "Error deleting file: " + filePath);
    } else {
        log(INFO, "File deleted successfully: " + filePath);
    }
}

std::string deleteFileHandler(const std::string& httpRequest) {
    std::string httpResponse;
    if (checkSession(httpRequest)) {
        deleteFile("sergsysoev%40gmail.com/images.jpeg");
    } else {
        httpResponse = getNotFoundResponse();
    }
    
    return "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/home\r\n\r\n";;
}

std::string downloadHandler(const std::string& httpRequest){
    std::string httpResponse;
    if (checkSession(httpRequest)) {
        httpResponse = sendFile("sergsysoev%40gmail.com/images.jpeg");
    } else {
        httpResponse = getNotFoundResponse();
    }
    
    return httpResponse;
}

// Обработка данных от клиента
void handleClient(int clientSocket) {
    char buffer[1048576];
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
    } else if (httpPath == "/") {
        httpResponse = rootHandler(httpRequest);
    } else if (httpPath == "/signout") {
        httpResponse = signoutHandler(httpRequest);
    } else if (httpPath == "/download") {
        httpResponse = downloadHandler(httpRequest);
    } else if (httpPath == "/delete") {
        httpResponse = deleteFileHandler(httpRequest);
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
