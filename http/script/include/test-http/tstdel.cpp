#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>

// Путь к директории, в которой хранятся файлы
const char* file_dir = "tmp/";

// Функция для вывода HTML-заголовка
void printHeader() {
    std::cout << "Content-Type: text/html\n\n";
    std::cout << "<html><head><title>List files</title></head><body>";
    std::cout << "<h1>List files in tmp</h1>";
}

// Функция для вывода списка файлов
void listFiles() {
    printHeader();
    std::cout << "<ul>";
    struct dirent* entry;
    struct stat statbuf;
    DIR* dp = opendir(file_dir);
    
    if (dp != NULL) {
        while ((entry = readdir(dp))) {
            std::string filename = entry->d_name;
            
            if (filename != "." && filename != "..") {
                std::cout << "<li><a href=\"?file=" << filename << "\">" << filename << "</a> - <a href=\"?delete=" << filename << "\">Delete</a></li>";
            }
        }
        closedir(dp);
    }
    
    std::cout << "</ul>";
    std::cout << "</body></html>";
}

// Функция для вывода содержимого файла
void viewFile(const char* filename) {
    std::string fullFilePath = std::string(file_dir) + filename;
    std::ifstream file(fullFilePath.c_str());

    if (file.is_open()) {
        printHeader();
        std::cout << "<pre>";
        std::string line;

        while (getline(file, line)) {
            std::cout << line << "\n";
        }

        std::cout << "</pre>";
        std::cout << "</body></html>";
        file.close();
    } else {
        // Файл не найден
        std::cout << "Content-Type: text/html\n\n";
        std::cout << "<html><body><p>File not found.</p></body></html>";
    }
}

// Функция для удаления файла
void deleteFile(const char* filename) {
    std::string fullFilePath = std::string(file_dir) + filename;
    
    if (unlink(fullFilePath.c_str()) == 0) {
        // Файл успешно удален
        std::cout << "Content-Type: text/html\n\n";
        std::cout << "<html><body><p>File '" << filename << "' delete success.</p></body></html>";
    } else {
        // Ошибка при удалении файла
        std::cout << "Content-Type: text/html\n\n";
        std::cout << "<html><body><p>Delete Err '" << filename << "'.</p></body></html>";
    }
}

int main() {
    const char* query = getenv("QUERY_STRING");

    if (query) {
        if (strncmp(query, "file=", 5) == 0) {
            const char* filename = query + 5;
            viewFile(filename);
        } else if (strncmp(query, "delete=", 7) == 0) {
            const char* filename = query + 7;
            deleteFile(filename);
        } else {
            listFiles();
        }
    } else {
        listFiles();
    }

    return 0;
}
