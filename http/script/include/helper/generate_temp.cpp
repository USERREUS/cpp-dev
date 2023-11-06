#include <iostream>
#include <string>
#include <random>

// Функция для генерации случайной строки
std::string generateRandomString(int length) {
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

int main() {
    std::string randomString = generateRandomString(8);
    std::cout << "Random String: " << randomString << std::endl;

    return 0;
}
