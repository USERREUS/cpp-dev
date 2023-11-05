#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <unistd.h>

struct UploadedFile {
    std::string filename;
    std::string type;
    std::string tmp_name;
    int error;
    int size;
};

// Функция для разбора multipart/form-data данных
void parseMultipartFormData(const std::string& data, const std::string& boundary, std::vector<UploadedFile>& files) {
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

    // Создать временную директорию для хранения файлов
    char tempDirTemplate[] = "tmp/uploadXXXXXX";
    char* tempDirPath = mkdtemp(tempDirTemplate);

    if (tempDirPath == nullptr) {
        std::cerr << "Failed to create a temporary directory." << std::endl;
        return;
    }

    // Обработка каждой части данных
    for (const std::string& part : parts) {
        // Найдите Content-Disposition и данные внутри каждой части
        size_t contentDispositionPos = part.find("Content-Disposition: form-data;");
        if (contentDispositionPos != std::string::npos) {
            UploadedFile file;
            size_t nameStart = part.find("name=\"", contentDispositionPos) + 6;
            size_t nameEnd = part.find("\"", nameStart);
            file.filename = part.substr(nameStart, nameEnd - nameStart);

            size_t filenameStart = part.find("filename=\"", contentDispositionPos);
            if (filenameStart != std::string::npos) {
                filenameStart += 10;
                size_t filenameEnd = part.find("\"", filenameStart);
                file.tmp_name = tempDirPath;  // Сохранить файл во временной директории
                file.tmp_name += "/";
                file.tmp_name += part.substr(filenameStart, filenameEnd - filenameStart);

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
                    std::cerr << "Failed to open temporary file for writing." << std::endl;
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

                // Вывод информации о поле без файла
                std::cout << "Field Name: " << file.filename << "\n";
                std::cout << "Field Data:\n" << fieldData << "\n";
            }

            // Добавление структуры UploadedFile в вектор файлов
            files.push_back(file);
        }
    }

    // Удалить временную директорию и ее содержимое
    // if (tempDirPath) {
    //     std::string removeTempDirCmd = "rm -rf " + std::string(tempDirPath);
    //     system(removeTempDirCmd.c_str());
    // }
}

int main() {
    // Получение типа контента (Content-Type)
    const char* contentType = getenv("CONTENT_TYPE");

    if (contentType && std::string(contentType).find("multipart/form-data") != std::string::npos) {
        // Получение размера контента
        const char* contentLengthStr = getenv("CONTENT_LENGTH");
        if (contentLengthStr) {
            size_t contentLength = std::stoul(contentLengthStr);

            // Буфер для хранения данных
            char* dataBuffer = new char[contentLength];

            // Чтение данных из стандартного ввода (stdin)
            if (std::cin.read(dataBuffer, contentLength)) {
                // Преобразование данных в строку
                std::string postData(dataBuffer, contentLength);

                // Получение границы (boundary)
                size_t boundaryPos = std::string(contentType).find("boundary=");
                if (boundaryPos != std::string::npos) {
                    std::string boundary = std::string(contentType).substr(boundaryPos + 9);

                    // Вектор для хранения файлов
                    std::vector<UploadedFile> files;

                    std::cout << "Content-Type: text/plain\r\n\r\n";

                    // Разбор multipart/form-data данных
                    parseMultipartFormData(postData, boundary, files);

                    // Вывод информации о загруженных файлах
                    for (const UploadedFile& file : files) {
                        std::cout << "Uploaded File:\n";
                        std::cout << "File Name: " << file.filename << "\n";
                        std::cout << "Temporary Name: " << file.tmp_name << "\n";
                        std::cout << "MIME Type: " << file.type << "\n";
                        std::cout << "Size: " << file.size << " bytes\n";
                        std::cout << "Error Code: " << file.error << "\n";
                    }
                } else {
                    std::cout << "Content-Type: text/plain\r\n\r\n";
                    std::cout << "Boundary not found in Content-Type header.\n";
                }
            } else {
                std::cout << "Content-Type: text/plain\r\n\r\n";
                std::cout << "Failed to read data.\n";
            }

            delete[] dataBuffer;
        } else {
            std::cout << "Content-Type: text/plain\r\n\r\n";
            std::cout << "Content-Length not provided.\n";
        }
    } else {
        std::cout << "Content-Type: text/plain\r\n\r\n";
        std::cout << "Unsupported Content-Type: " << (contentType ? contentType : "N/A") << "\n";
    }

    return 0;
}
