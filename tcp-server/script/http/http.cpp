#include "http.hpp"

HTTP::HTTP(const std::string& httpRequest) : httpRequest(httpRequest) {

    try {
        tempDir = std::filesystem::current_path() / "temp";
        std::filesystem::create_directory(tempDir);
    } catch (const std::exception& e) {
        Helper::log(ERROR, e.what());
    }

    headers[COOKIE]         = Helper::extractHeader(httpRequest, COOKIE);
    headers[CONTENT_LENGTH] = Helper::extractHeader(httpRequest, CONTENT_LENGTH);
    headers[CONTENT_TYPE]   = Helper::extractHeader(httpRequest, CONTENT_TYPE);
    headers[HTTP_METHOD]    = Helper::extractHttpMethod(httpRequest);
    headers[PATH]           = Helper::extractPath(httpRequest);
    headers[QUERY_STRING]   = Helper::extractQueryString(httpRequest);

    Helper::log(INFO, 
        "Method: " + headers[HTTP_METHOD] + 
        "; path: " + headers[PATH] + 
        "; query: " + headers[QUERY_STRING] + 
        "; cookie: " + headers[COOKIE] + 
        "; content-type: " + headers[CONTENT_TYPE] + 
        "; content-lenght: " + headers[CONTENT_LENGTH]
    );

    cookieData = Helper::parseCookies(headers[COOKIE]);
    std::string method = headers[HTTP_METHOD];

    if (method == GET) {
        getData = Helper::parseForm(headers[QUERY_STRING]);
    } else if (method == POST) {
        std::string ctype = headers[CONTENT_TYPE];
        if (ctype.find(URL_ENCODED) != std::string::npos) {
            isURLEncoded = true;
            postData = Helper::parseForm(getPostData());
        } else if (ctype.find(MULTIPART) != std::string::npos) {
            size_t boundaryStart = ctype.find(BOUNDARY);
            boundary = ctype.substr(boundaryStart + BOUNDARY.length());
            isMultipart = true;
            MultipartHandler();
        }
    }
}

void HTTP::moveFiles(const std::string& dir) {
    for(auto file : filesData) {
        if (!move_uploaded_file(file.second, dir)) {
            Helper::log(ERROR, "moveFiles: " + file.first);
        }
        Helper::log(INFO, "moveFiles: " + file.first);
    }
}

UploadedFile HTTP::getFile(std::string name) { return filesData.find(name) != filesData.end() ? filesData[name] : UploadedFile{}; }

bool HTTP::move_uploaded_file(UploadedFile file, const std::string& path) {
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
    }
}

std::map<std::string, UploadedFile> HTTP::parseMultipartFormData(const std::string& data, const std::string boundary) {
    std::map<std::string, UploadedFile> filesData;

    std::vector<std::string> parts;
    size_t startPos = 0;
    size_t endPos = 0;

    while (true) {
        endPos = data.find(boundary + "--", startPos);
        if (endPos == std::string::npos) {
            if (parts.size() > 0) {
                parts[parts.size() - 1] = parts[parts.size() - 1].substr(0, parts[parts.size() - 1].length() - 4);
            }
            break;
        }

        std::string part = data.substr(startPos, endPos - startPos);
        parts.push_back(part);

        startPos = endPos + boundary.size() + 2; // Skip boundary and CRLF
    }
    Helper::log(DEBUG, "parts.size(): " + std::to_string(parts.size()));

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
                    Helper::log(ERROR, "parseMultipartFormData: !fileStream.is_open()");
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
            filesData[file.filename] = file;
        }
    }

    return filesData;
}

std::string HTTP::httpPOST(const std::string& name) {
    return postData.find(name) == postData.end() ? "" : postData[name];
}

std::string HTTP::httpGET(const std::string& name) {
    return getData.find(name) == getData.end() ? "" : getData[name];
}

void HTTP::MultipartHandler() {
    Helper::log(DEBUG, "MultipartHandler");

    std::string postData = getPostData();
    filesData = parseMultipartFormData(postData, boundary);
}

std::string HTTP::getHeader(const std::string& name) { return headers.find(name) != headers.end() ? headers[name] : ""; }

// Функция для обработки POST-запроса
std::string HTTP::getPostData() {
    Helper::log(DEBUG, "getPostData");
    std::string contentType         = headers[CONTENT_TYPE];
    std::string httpContentLength   = headers[CONTENT_LENGTH];
    size_t contentLength            = std::stoi(httpContentLength);
    Helper::log(DEBUG, "getPostData: contentLength = " + std::to_string(contentLength));
    size_t bodyStart                = httpRequest.find("\r\n\r\n") + 4;
    if (bodyStart == std::string::npos) {
        Helper::log(ERROR, "getPostData: bodyStart == std::string::npos");
    }
    std::string postData            = httpRequest.substr(bodyStart, contentLength);
    return postData;
}

std::string HTTP::getCookie(const std::string& name) { return cookieData.find(name) != cookieData.end() ? cookieData[name] : ""; }