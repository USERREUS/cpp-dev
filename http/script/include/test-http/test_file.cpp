#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>

struct UploadedFile {
    std::string filename;
    std::string type;
    std::string tmp_name;
    int error;
    int size;
};

// Определите директорию, в которую вы хотите сохранить загруженные файлы
std::string tempDir = "";

// Вектор для хранения временных имен файлов
std::vector<std::string> tempFileNames;

// Функция для сохранения файла во временную директорию
void saveFileToTempDirectory(const std::string& tmpFileName, const std::string& fileData) {
    std::string tempFilePath = tempDir + tmpFileName;
    std::ofstream outputFile(tempFilePath, std::ios::binary);

    if (outputFile) {
        outputFile.write(fileData.c_str(), fileData.size());
        outputFile.close();

        //std::cout << "Saved file: " << tmpFileName << " (Size: " << fileData.size() << " bytes)\n";
    } else {
        //std::cerr << "Failed to save file: " << tmpFileName << "\n";
    }
}

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
                file.tmp_name = part.substr(filenameStart, filenameEnd - filenameStart);

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

                // Здесь можно обработать и сохранить файл, используя временное имя
                if (!file.tmp_name.empty()) {
                    // Путь к временной директории, где будут сохраняться файлы
                    std::string tempDir = "tmp/"; // Замените на нужный путь

                    // Полный путь к файлу во временной директории
                    std::string filePath = tempDir + file.tmp_name;

                    // Создаем файл в бинарном режиме для записи
                    std::ofstream outputFile(filePath, std::ios::binary);

                    if (outputFile) {
                        // Записываем данные в файл
                        size_t dataStart = part.find("\r\n\r\n", contentDispositionPos) + 4;
                        outputFile.write(part.data() + dataStart, part.size() - dataStart);

                        outputFile.close();

                        std::cout << "Saved file: " << filePath << " (Size: " << file.size << " bytes)\n";
                    } else {
                        std::cerr << "Failed to save file: " << filePath << "\n";
                    }
                }

                // file.tmp_name содержит имя файла, а file.size - его размер
                // Можно также сохранить данные в файл, если они находятся в части данных part
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
                //std::cout << "Field Name: " << file.filename << "\n";
                //std::cout << "Field Data:\n" << fieldData << "\n";
            }

            // Добавление структуры UploadedFile в вектор файлов
            files.push_back(file);
        }
    }
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
