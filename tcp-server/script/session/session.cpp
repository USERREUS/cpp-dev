#include "session.hpp"

Session::Session() {
    UUID = generateUUID();
}

Session::Session(std::string UUID) {
    this->UUID = UUID;
}

std::string Session::getUUID() {
    return UUID;
}

void Session::setUUID(std::string UUID) {
    this->UUID = UUID;
}

void Session::set(std::string name, std::string value) {
    client.Set(UUID + name, value);
}

std::string Session::get(std::string name) {
    return client.Get(UUID + name);
}

Session::~Session() {}

bool Session::IsValid() { return get(LOGIN) == "" ? false : true; }

std::string Session::generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(1, 100);
    std::string uuid = std::string(36, ' ');
    int rnd = 0;

    uuid[8] = '-';
    uuid[13] = '-';
    uuid[18] = '-';
    uuid[23] = '-';

    uuid[14] = '4';

    for (int i = 0; i < 36; i++) {
        if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
            if (rnd <= 0x02) {
                rnd = (0x2000000 + (distribution(gen) * 0x1000000)) | 0;
            }
            rnd >>= 4;
            uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
        }
    }

    return uuid;
}
