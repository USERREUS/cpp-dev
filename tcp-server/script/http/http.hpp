#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <map>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include "../helper/helper.hpp"

typedef struct {
    std::string filename; // реальное имя файла
    std::string type; // MIME-тип файла
    std::string tmp_name; // временное имя файла
    int error; // код ошибки (0, если нет)
    int size; // размер загружаемого файла
} UploadedFile;

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
void log(LogLevel level, const std::string& message);

class HTTP {
public:
    HTTP(const std::string& httpRequest);
    UploadedFile getFile(std::string name);
    ~HTTP();

private:
    std::string httpRequest;
    std::map<std::string, UploadedFile> filesData;

    bool isMultipart = false;
    std::filesystem::path tempDir;
    std::string boundary;

    std::map<std::string, UploadedFile> parseMultipartFormData(const std::string& data, const std::string boundary);
    void checkMultipart();
    void MultipartHandler();
    std::string handlePostData(const std::string& httpRequest);
    bool move_uploaded_file(const UploadedFile& file, const std::string& path);
};

#endif // HTTP_HPP
