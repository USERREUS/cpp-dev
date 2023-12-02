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

const std::string PAGE_NOT_FOUND = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 Not Found</title><style>body {font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh; } .container { text-align: center; } h1 { color: #333; } p { color: #666;}</style></head><body><div class=\"container\"><h1>404 Not Found</h1><p>The requested page could not be found.</p></div></body></html>";

const std::string CODE_NOT_FOUND_404 = "404 Not Found";
const std::string CODE_OK_200 = "200 OK";

const std::string HTML_FILEPATH_ROOT_UNAUTORIZED = "html/rootunauthorized.html";
const std::string HTML_FILEPATH_ROOT_AUTORIZED   = "html/rootauthorized.html";
const std::string HTML_FILEPATH_SIGNUP = "html/signup.html";
const std::string HTML_FILEPATH_SIGNIN = "html/signin.html";
const std::string HTML_FILEPATH_SIGNUP_ERR = "html/signuperr.html";
const std::string HTML_FILEPATH_SIGNIN_ERR = "html/signinerr.html";

const std::string REDIRECT_SIGNIN   = "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/signin\r\n\r\n";
const std::string REDIRECT_HOME     = "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/home\r\n";

std::string generateHTTPResponse(const std::string& code, const std::string& html)
{
    std::string httpResponse = "HTTP/1.0 " + code + "\r\n";
    httpResponse += "Content-Type: text/html\r\n";
    httpResponse += "Content-Length: " + std::to_string(html.size()) + "\r\n\r\n";
    httpResponse += html;
    return httpResponse;
}

std::string getResponse(const std::string& fileName) {
    std::string httpResponse, htmlResponse;
    htmlResponse = Helper::readFile(fileName);
    if (!htmlResponse.empty()) {
        httpResponse = generateHTTPResponse(CODE_OK_200, htmlResponse);
    } else {
        httpResponse = generateHTTPResponse(CODE_NOT_FOUND_404, PAGE_NOT_FOUND);
    }
    return httpResponse;
}

std::string getHomePage(const std::string& login) {
    std::string htmlContent = "<!DOCTYPE html>\n"
                              "<html lang=\"en\">\n"
                              "<head>\n"
                              "    <meta charset=\"UTF-8\">\n"
                              "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                              "    <title>Home</title>\n"
                              "</head>\n"
                              "<body>\n"
                              "    <h1>Welcome to the Home Page!</h1>\n"
                              "    <p>Hello, " + login + "! This is your home page.</p>\n"
                              "</body>\n"
                              "</html>\n";

    return generateHTTPResponse(CODE_OK_200, htmlContent);
}

// bool signinDataValidation(const std::string& httpRequest) {
//     std::string postData = handlePostData(httpRequest);
//     auto auth_params = Helper::parseForm(postData);
//     if (auth_params.find("email") != auth_params.end() && auth_params.find("password") != auth_params.end()) {
//         Store store = Store();
//         return store.dataValidation(auth_params["email"], auth_params["password"]);
//     }
//     return false;
// }

std::string rootHandler(Session& session) { 
    if(session.IsValid()) {
        Helper::log(INFO, "Valid Session");
        return getResponse(HTML_FILEPATH_ROOT_AUTORIZED);
    }
    Helper::log(INFO, "Not valid Session");
    return getResponse(HTML_FILEPATH_ROOT_UNAUTORIZED);
}
//std::string homeHandler(HTTP* http) { return http->IsValidSession() ? generateHTTPResponse(http_cns[OK_200], generateFileList(http->getSessionVal("login"))) : generateHTTPResponse(http_cns[NOT_FOUND_404], html_pns[NOT_FOUND]); }

std::string signinValidationHandler(HTTP& http, Store& store) {
    std::string httpMethod = http.getHeader(HTTP_METHOD);
    if (httpMethod == POST) {
        std::string login = http.httpPOST(LOGIN);
        std::string pass  = http.httpPOST(PASSWORD);
        if (pass != "" && login != "") {
            std::map<std::string, std::string> signup_params;
            signup_params[LOGIN]    = login;
            signup_params[PASSWORD] = pass;
            if (store.SignIN(signup_params)) {
                Session session;
                session.set(LOGIN, login);
                return REDIRECT_HOME + "Set-Cookie: session=" + session.getUUID() + "; path=/\r\n\r\n";
            }
            Helper::log(ERROR, "signinValidationHandler: incorrect params");
            return getResponse(HTML_FILEPATH_SIGNIN_ERR);
        }
    }

    return generateHTTPResponse(CODE_NOT_FOUND_404, PAGE_NOT_FOUND);
}

// void makeUserDir(const std::string& name) {
//     if (mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0) {
//         log(INFO, "Directory created: " + name);
//     } else {
//         log(ERROR, "Error creating directory");
//     }
// }

// bool signupDataValidation(const std::string& httpRequest) {
//     Store store = Store();
//     std::string postData = handlePostData(httpRequest);
//     auto params = Helper::parseForm(postData);
//     if (params.find("email") == params.end() || !store.emailValidation(params["email"])) {
//         return false;
//     }
//     makeUserDir(params["email"]);
//     store.AppendData(postData);
//     return true;
// }

std::string signupValidationHandler(HTTP& http, Store& store) {
    std::string httpMethod = http.getHeader(HTTP_METHOD);
    if (httpMethod == POST) {
        std::string login = http.httpPOST(LOGIN);
        std::string pass  = http.httpPOST(PASSWORD);
        if (pass != "" && login != "") {
            std::map<std::string, std::string> signup_params;
            signup_params[LOGIN]    = login;
            signup_params[PASSWORD] = pass;
            if (store.SignUP(signup_params)) {
                return REDIRECT_SIGNIN;
            }
            Helper::log(ERROR, "signupValidationHandler: incorrect params");
            return getResponse(HTML_FILEPATH_SIGNUP_ERR);
        }
    }
    return generateHTTPResponse(CODE_NOT_FOUND_404, PAGE_NOT_FOUND);
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

// std::string sendFile(const std::string& filePath) {
//     std::ifstream file(filePath, std::ios::binary);
//     if (!file.is_open()) {
//         log(ERROR, "Error opening file: " + filePath);
//         return getNotFoundResponse();
//     }

//     std::ostringstream fileContent;
//     fileContent << file.rdbuf();
//     file.close();

//     std::string response = "HTTP/1.1 200 OK\r\n";
//     response += "Content-Type: application/octet-stream\r\n";
//     response += "Content-Disposition: attachment; filename=\"" + std::filesystem::path(filePath).filename().string() + "\"\r\n";
//     response += "Content-Length: " + std::to_string(fileContent.str().size()) + "\r\n";
//     response += "\r\n" + fileContent.str();

//     return response;
// }

// void deleteFile(const std::string& filePath) {
//     if (remove(filePath.c_str()) != 0) {
//         log(ERROR, "Error deleting file: " + filePath);
//     } else {
//         log(INFO, "File deleted successfully: " + filePath);
//     }
// }

// std::string deleteFileHandler(const std::string& httpRequest) {
//     std::string httpResponse;
//     if (checkSession(httpRequest)) {
//         deleteFile("sergsysoev%40gmail.com/images.jpeg");
//     } else {
//         httpResponse = getNotFoundResponse();
//     }
    
//     return "HTTP/1.0 302 Found\r\nLocation: http://localhost:8080/home\r\n\r\n";;
// }

// std::string downloadHandler(const std::string& httpRequest){
//     std::string httpResponse;
//     if (checkSession(httpRequest)) {
//         httpResponse = sendFile("sergsysoev%40gmail.com/images.jpeg");
//     } else {
//         httpResponse = getNotFoundResponse();
//     }
    
//     return httpResponse;
// }

// Обработка данных от клиента
void handleClient(int clientSocket) {
    char buffer[1048576]; //1 MB
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        Helper::log(ERROR, "Ошибка при чтении данных от клиента");
        close(clientSocket);
        return;
    }

    // Разбиение HTTP-запроса на заголовки
    std::string httpRequest(buffer, bytesRead);

    HTTP http(httpRequest);
    std::string httpResponse;
    std::string httpPath = http.getHeader(PATH);
    std::string uuid = http.getCookie(SESSION_UUID);
    Helper::log(INFO, uuid);
    Session session(uuid);
    Store store;

    if (httpPath == "/signup") {
        httpResponse = getResponse(HTML_FILEPATH_SIGNUP);
    } else if (httpPath == "/signup/validation") {
        httpResponse = signupValidationHandler(http, store);
    } else if (httpPath == "/signin") {
        httpResponse = getResponse(HTML_FILEPATH_SIGNIN);
    } else if (httpPath == "/signin/validation") {
        httpResponse = signinValidationHandler(http, store);
    } else if (httpPath == "/home") {
    //    httpResponse = homeHandler(http);
    } else if (httpPath == "/") {
        httpResponse = rootHandler(session);
    } else if (httpPath == "/signout") {
        httpResponse = signoutHandler(httpRequest);
    } else if (httpPath == "/download") {
    //    httpResponse = downloadHandler(httpRequest);
    } else if (httpPath == "/delete") {
    //    httpResponse = deleteFileHandler(httpRequest);
    } else {
        httpResponse = generateHTTPResponse(CODE_NOT_FOUND_404, PAGE_NOT_FOUND);
    }

    ssize_t bytesSent = send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);

    if (bytesSent == -1) {
        Helper::log(ERROR, "Ошибка при отправке данных клиенту");
    }

    // Закрытие сокета клиента
    close(clientSocket);
}

int main() {
    // Создание сокета
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        Helper::log(ERROR, "Ошибка создания сокета");
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
        Helper::log(ERROR, "Ошибка привязки сокета");
        close(serverSocket);
        return -1;
    }

    // Прослушивание порта
    if (listen(serverSocket, 10) == -1) {
        Helper::log(ERROR, "Ошибка прослушивания порта");
        close(serverSocket);
        return -1;
    }

    Helper::log(INFO, "Сервер запущен. Ожидание подключений...");

    // Принятие соединений и обработка данных
    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Принятие соединения
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            Helper::log(ERROR, "Ошибка при принятии соединения");
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::string ipAddress(clientIP);
        Helper::log(INFO, "Подключен клиент: " +  ipAddress);

        // Обработка данных от клиента
        handleClient(clientSocket);
    }

    // Закрытие серверного сокета
    close(serverSocket);

    return 0;
}
