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
#include "../session/session.hpp"
#include "../store/store.hpp"

typedef struct {
    std::string filename; // реальное имя файла
    std::string type; // MIME-тип файла
    std::string tmp_name; // временное имя файла
    int error; // код ошибки (0, если нет)
    int size; // размер загружаемого файла
} UploadedFile;

const std::string SESSION_UUID = "session";

const std::string GET = "GET";
const std::string POST = "POST";

const std::string HTTP_METHOD = "http_method";
const std::string COOKIE = "Cookie";
const std::string CONTENT_LENGTH = "Content-Length";
const std::string PATH = "Path";
const std::string QUERY_STRING = "query_string";
const std::string CONTENT_TYPE = "Content-Type";

const std::string URL_ENCODED = "x-www-form-urlencoded";
const std::string MULTIPART = "multipart/form-data";
const std::string BOUNDARY = "boundary=";

class HTTP {
public:
    HTTP(const std::string& httpRequest);
    UploadedFile getFile(std::string name);
    void moveFiles(const std::string& dir);
    std::string getHeader(const std::string& name);
    std::string getCookie(const std::string& name);
    std::string httpPOST(const std::string& name);
    std::string httpGET(const std::string& name);
    ~HTTP();

private:
    std::string httpRequest;
    std::map <std::string, std::string> headers;
	std::map <std::string, std::string> getData;
	std::map <std::string, std::string> postData;
	std::map <std::string, std::string> cookieData;
    std::map<std::string, UploadedFile> filesData;

    bool isMultipart = false;
    bool isURLEncoded = false;
    std::filesystem::path tempDir;
    std::string boundary;

    std::map<std::string, UploadedFile> parseMultipartFormData(const std::string& data, const std::string boundary);
    void checkMultipart();
    void checkURLEncoded();
    void MultipartHandler();
    bool move_uploaded_file(UploadedFile file, const std::string& path);
    std::string getPostData();
};

#endif // HTTP_HPP
