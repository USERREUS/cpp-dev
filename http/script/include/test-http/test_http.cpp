#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <map>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <random>

typedef struct {
    std::string filename; // реальное имя файла
    std::string type; // MIME-тип файла
    std::string tmp_name; // временное имя файла
    int error; // код ошибки (0, если нет)
    int size; // размер загружаемого файла
} UploadedFile;

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
};

class HTTP {
public:
	HTTP() {
        headers["QUERY_STRING"]     = Helper::getenvHandler("QUERY_STRING");
        headers["REQUEST_METHOD"]   = Helper::getenvHandler("REQUEST_METHOD");
        headers["HTTP_COOKIE"]      = Helper::getenvHandler("HTTP_COOKIE");
        headers["CONTENT_TYPE"]     = Helper::getenvHandler("CONTENT_TYPE");
        headers["CONTENT_LENGTH"]   = Helper::getenvHandler("CONTENT_LENGTH");

        cookieData  = Helper::parseCookies(getHeader("HTTP_COOKIE"));
        getData     = Helper::parseForm(getHeader("QUERY_STRING"));

        std::cout << "Content-Type: text/html" << std::endl << std::endl;
        std::cout << "<p>Test</p>" << std::endl;

        // Создать временную директорию для хранения файлов
        char tempDirTemplate[] = "tmpXXXXXX";
        tempDirPath = mkdtemp(tempDirTemplate);

        if (tempDirPath == nullptr) {
            std::cout << "<p>Failed to create a temporary directory.</p>" << std::endl;
        }

        checkMultipart();

        if (getHeader("REQUEST_METHOD") == "POST") {
            if (getHeader("CONTENT_TYPE").find("x-www-form-urlencoded") != std::string::npos) {
                std::string body;
                std::cin >> body;
                postData = Helper::parseForm(body);
            } 
            else if (isMultipart) {
                MultipartHandler();
                if (move_uploaded_file(getFile("smile.png"), "src")) {
                    std::cout << "Success" << std::endl;
                }
            } 
            else {
                //TODO
            }
        }
    }

    std::string getHeader(std::string name) { return headers.find(name) != headers.end() ? headers[name] : ""; }
    std::string httpGet(std::string name) { return getData.find(name) != getData.end() ? getData[name] : ""; }
    std::string httpPost(std::string name) { return postData.find(name) != postData.end() ? postData[name] : ""; }
    std::string getCookie(std::string name) { return cookieData.find(name) != cookieData.end() ? cookieData[name] : ""; }
    UploadedFile getFile(std::string name) { return filesData.find(name) != filesData.end() ? filesData[name] : UploadedFile{}; }
    
    void setCookie(std::string name, std::string value) { std::cout << "Set-Cookie: " + name + "=" + value + "; \n"; }
    
    // Функция для перемещения загруженного файла
    bool move_uploaded_file(const UploadedFile& file, const std::string& path) {
        if (file.error == 0 && !file.tmp_name.empty()) {
            std::string source = file.tmp_name;
            std::string destination = path + "/" + file.filename;

            if (rename(source.c_str(), destination.c_str()) == 0) {
                return true;
            }
        }

        return false;
    }

    // Функция для вывода одной пары ключ-значение в HTML-таблицу
    void outputKeyValue(const std::string& key, const std::string& value) {
        std::cout << "<tr><td>" << key << "</td><td>" << value << "</td></tr>\n";
    }
    void HTML() {
        // Устанавливаем заголовок Content-Type для вывода HTML
        std::cout << "Content-Type: text/html\r\n\r\n";
        std::cout << "<html><head><title>CGI Output</title></head><body>\n";

        // Выводим заголовок
        std::cout << "<h1>Headers:</h1><table>";
        for (const auto& pair : headers) {
            outputKeyValue(pair.first, pair.second);
        }
        std::cout << "</table>";

        // Выводим данные GET-запроса
        std::cout << "<h1>GET Data:</h1><table>";
        for (const auto& pair : getData) {
            outputKeyValue(pair.first, pair.second);
        }
        std::cout << "</table>";

        // Выводим данные POST-запроса
        std::cout << "<h1>POST Data:</h1><table>";
        for (const auto& pair : postData) {
            outputKeyValue(pair.first, pair.second);
        }
        std::cout << "</table>";

        // Выводим данные cookie
        std::cout << "<h1>Cookie Data:</h1><table>";
        for (const auto& pair : cookieData) {
            outputKeyValue(pair.first, pair.second);
        }
        std::cout << "</table>";

        // Выводим информацию о файлах (если есть)
        if (isMultipart) {
            std::cout << "<h1>Multipart Data:</h1>";
            std::cout << "<p>Boundary: " << boundary << "</p>";

            if (!filesData.empty()) {
                std::cout << "<h2>Uploaded Files:</h2>";
                for (const auto& pair : filesData) {
                    std::cout << "<p>Field Name: " << pair.first << "<br>File Name: " << pair.second.filename << "</p>";
                }
            }
        }

        std::cout << "</body></html>";
    }

    ~HTTP() {
        //Удалить временную директорию и ее содержимое
        if (tempDirPath) {
            std::string removeTempDirCmd = "rm -rf " + std::string(tempDirPath);
            system(removeTempDirCmd.c_str());
        }
    }

private:
    std::map <std::string, std::string> headers;
    std::map <std::string, std::string> getData;
    std::map <std::string, std::string> postData;
    std::map <std::string, std::string> cookieData;
    std::map <std::string, UploadedFile> filesData;
    
    bool isMultipart = false;
    char* tempDirPath = nullptr;
    std::string boundary;
    
    std::map <std::string, UploadedFile> parseMultipartFormData(const std::string &data, const std::string boundary) {
        std::map <std::string, UploadedFile> filesData;
        
        std::vector<std::string> parts;
        size_t startPos = 0;
        size_t endPos = 0;

        while (true) {
            endPos = data.find("--" + boundary, startPos);
            if (endPos == std::string::npos) {
                break;
            }

            std::string part = data.substr(startPos, endPos - startPos);
            parts.push_back(part);

            startPos = endPos + boundary.size() + 2; // Skip boundary and CRLF
        }

        // Обработка каждой части данных
        for (const std::string& part : parts) {
            // Найдите Content-Disposition и данные внутри каждой части
            size_t contentDispositionPos = part.find("Content-Disposition: form-data;");
            if (contentDispositionPos != std::string::npos) {
                UploadedFile file;
                // size_t nameStart = part.find("name=\"", contentDispositionPos) + 6;
                // size_t nameEnd = part.find("\"", nameStart);
                // file.filename = part.substr(nameStart, nameEnd - nameStart);

                size_t filenameStart = part.find("filename=\"", contentDispositionPos);
                if (filenameStart != std::string::npos) {
                    filenameStart += 10;
                    size_t filenameEnd = part.find("\"", filenameStart);
                    file.filename = part.substr(filenameStart, filenameEnd - filenameStart);
                    file.tmp_name = tempDirPath;  // Сохранить файл во временной директории
                    file.tmp_name += "/";
                    file.tmp_name += Helper::generateRandomString(8) + file.filename;
                    // Извлечение MIME-типа
                    size_t contentTypePos = part.find("Content-Type:", contentDispositionPos);
                    if (contentTypePos != std::string::npos) {
                        size_t contentTypeStart = part.find(" ", contentTypePos) + 1;
                        size_t contentTypeEnd = part.find("\r\n", contentTypeStart);
                        file.type = part.substr(contentTypeStart, contentTypeEnd - contentTypeStart);
                    }
                    // Извлечение размера файла
                    file.size = part.size() - part.find("\r\n\r\n", contentDispositionPos) - 4;
                    // Установка ошибки в 0 (нет ошибки)
                    file.error = 0;
                    // Открываем временный файл для записи
                    std::ofstream fileStream(file.tmp_name, std::ios::out | std::ios::binary);

                    if (!fileStream.is_open()) {
                        std::cout << "<p>Failed to open temporary file for writing.</p>" << std::endl;
                        continue;
                    }
                    // Записываем данные во временный файл
                    const char* fileData = part.c_str() + part.find("\r\n\r\n", contentDispositionPos) + 4;
                    fileStream.write(fileData, file.size);
                    fileStream.close();
                } else {
                    // Извлечение данных (значения) для полей без файла
                    size_t dataStart = part.find("\r\n\r\n", contentDispositionPos) + 4;
                    std::string fieldData = part.substr(dataStart);
                    // Заполнение свойств для полей без файла
                    file.tmp_name = "";
                    file.type = "";
                    file.size = fieldData.size();
                    file.error = 0;
                }
                std::cout << "Uploaded File:\n";
                std::cout << "File Name: " << file.filename << "\n";
                std::cout << "Temporary Name: " << file.tmp_name << "\n";
                std::cout << "MIME Type: " << file.type << "\n";
                std::cout << "Size: " << file.size << " bytes\n";
                std::cout << "Error Code: " << file.error << "\n";
                // Добавление структуры UploadedFile в map файлов
                filesData[file.filename] = file;
            }
        }
        //Удалить временную директорию и ее содержимое
        // if (tempDirPath) {
        //     std::string removeTempDirCmd = "rm -rf " + std::string(tempDirPath);
        //     //system(removeTempDirCmd.c_str());
        //     std::cout << "<p>" << removeTempDirCmd << "</p>" << std::endl;
        // }

        return filesData;
    }
    void checkMultipart() {
        std::cout << "<p>checkMultipart</p>" << std::endl;

        std::string method = getHeader("REQUEST_METHOD");
        std::string ctype = getHeader("CONTENT_TYPE");
        if (method == "POST" && ctype.find("multipart/form-data") != std::string::npos) {
            size_t boundaryStart = ctype.find("boundary=");
            if (boundaryStart != std::string::npos) {
                boundary = ctype.substr(boundaryStart + strlen("boundary="));
                isMultipart = true;
            }
        }
    }
    void MultipartHandler() {
        std::cout << "<p>MultipartHandler</p>" << std::endl;

        size_t contentLength = std::stoul(getHeader("CONTENT_LENGTH"));
        char* dataBuffer = new char[contentLength];

        if (std::cin.read(dataBuffer, contentLength)) {

            std::string postData(dataBuffer, contentLength);
            filesData = parseMultipartFormData(postData, boundary);
        }

        delete[] dataBuffer;
    }
};

int main() {
    HTTP http = HTTP();
    //http.HTML();
    return 0;
}