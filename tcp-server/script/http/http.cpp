#include "http.hpp"

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

HTTP::HTTP(const std::string& httpRequest) : httpRequest(httpRequest) {
    // Create a temporary directory for storing files
    this->httpRequest = httpRequest;

    try {
        // Create a temporary directory
        tempDir = std::filesystem::current_path() / "temp";
        std::filesystem::create_directory(tempDir);

        log(INFO, "Temporary directory created: " + tempDir.string());
    } catch (const std::exception& e) {
        log(ERROR, e.what());
    }

    checkMultipart();
    MultipartHandler();
    if (move_uploaded_file(getFile("smile.png"), "sergsysoev%40gmail.com")) {
        log(INFO, "Success");
    }
}

UploadedFile HTTP::getFile(std::string name) {
    return filesData.find(name) != filesData.end() ? filesData[name] : UploadedFile{};
}

bool HTTP::move_uploaded_file(const UploadedFile& file, const std::string& path) {
    if (file.error == 0 && !file.tmp_name.empty()) {
        std::string source = file.tmp_name;
        std::string destination = path + "/" + file.filename;

        if (rename(source.c_str(), destination.c_str()) == 0) {
            return true;
        }
    }

    return false;
}

HTTP::~HTTP() {
    // Remove the temporary directory and its contents
    if (std::filesystem::exists(tempDir)) {
        std::filesystem::remove_all(tempDir);
        log(INFO, "Temporary directory deleted: " + tempDir.string());
    }
}

std::map<std::string, UploadedFile> HTTP::parseMultipartFormData(const std::string& data, const std::string boundary) {
    log(INFO, "parseMultipartFormData");

    std::map<std::string, UploadedFile> filesData;

    std::vector<std::string> parts;
    size_t startPos = 0;
    size_t endPos = 0;

    while (true) {
        endPos = data.find(boundary + "--", startPos);
        if (endPos == std::string::npos) {
            break;
        }

        std::string part = data.substr(startPos, endPos - startPos);
        parts.push_back(part);

        startPos = endPos + boundary.size() + 2; // Skip boundary and CRLF
    }

    for (const std::string& part : parts) {
        size_t contentDispositionPos = part.find("Content-Disposition: form-data;");
        if (contentDispositionPos != std::string::npos) {
            UploadedFile file;

            size_t filenameStart = part.find("filename=\"", contentDispositionPos);
            if (filenameStart != std::string::npos) {
                filenameStart += 10;
                size_t filenameEnd = part.find("\"", filenameStart);
                file.filename = part.substr(filenameStart, filenameEnd - filenameStart);
                file.tmp_name = tempDir / (Helper::generateRandomString(8) + file.filename);

                size_t contentTypePos = part.find("Content-Type:", contentDispositionPos);
                if (contentTypePos != std::string::npos) {
                    size_t contentTypeStart = part.find(" ", contentTypePos) + 1;
                    size_t contentTypeEnd = part.find("\r\n", contentTypeStart);
                    file.type = part.substr(contentTypeStart, contentTypeEnd - contentTypeStart);
                }

                file.size = part.size() - part.find("\r\n\r\n", contentDispositionPos) - 4;
                file.error = 0;

                std::ofstream fileStream(file.tmp_name, std::ios::out | std::ios::binary);

                if (!fileStream.is_open()) {
                    log(ERROR, "Failed to open temporary file for writing.");
                    continue;
                }

                const char* fileData = part.c_str() + part.find("\r\n\r\n", contentDispositionPos) + 4;
                fileStream.write(fileData, file.size);
                fileStream.close();
            } else {
                size_t dataStart = part.find("\r\n\r\n", contentDispositionPos) + 4;
                std::string fieldData = part.substr(dataStart);
                file.tmp_name = "";
                file.type = "";
                file.size = fieldData.size();
                file.error = 0;
            }

            log(INFO, "Uploaded File:");
            log(INFO, "File Name: " + file.filename);
            log(INFO, "Temporary Name: " + file.tmp_name);
            log(INFO, "MIME Type: " + file.type);
            log(INFO, "Size: " + std::to_string(file.size) + " bytes");
            log(INFO, "Error Code: " + std::to_string(file.error));

            filesData[file.filename] = file;
        }
    }

    return filesData;
}

void HTTP::checkMultipart() {
    std::string method = Helper::extractHttpMethod(httpRequest);
    std::string ctype = Helper::extractHeader(httpRequest, "Content-Type");
    if (method == "POST" && ctype.find("multipart/form-data") != std::string::npos) {
        size_t boundaryStart = ctype.find("boundary=");
        if (boundaryStart != std::string::npos) {
            boundary = ctype.substr(boundaryStart + strlen("boundary="));
            log(INFO, "Boundary: " + boundary);
            isMultipart = true;
        }
    }
}

void HTTP::MultipartHandler() {
    std::string postData = handlePostData(httpRequest);
    filesData = parseMultipartFormData(postData, boundary);
}

std::string HTTP::handlePostData(const std::string& httpRequest) {
    size_t pos = httpRequest.find("\r\n\r\n");
    return pos != std::string::npos ? httpRequest.substr(pos + 4) : "";
}
