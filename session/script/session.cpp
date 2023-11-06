#include <string>
#include <random>
#include <iostream>
#include <hiredis/hiredis.h>
#include <vector>
#include <map>

//docker run --name some-redis -d -p 6379:6379 redis
//docker run -it --rm --link some-redis:redis redis redis-cli -h redis
//docker exec -it <container_name_or_id> redis-cli
//g++ -o session session.cpp -lhiredis

class RedisClient {
    redisContext* context_;

public:
    RedisClient(const std::string& host = "127.0.0.1", int port = 6379) {
        context_ = redisConnect(host.c_str(), port);
        if (context_ == nullptr || context_->err) {
            if (context_) {
                std::cerr << "Ошибка подключения к Redis: " << context_->errstr << std::endl;
            } else {
                std::cerr << "Не удалось выделить память для redisContext" << std::endl;
            }
            exit(1);
        }
    }
    ~RedisClient() {
        if (context_) {
            redisFree(context_);
        }
    }
    void Set(const std::string& key, const std::string& value) {
        redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %s", key.c_str(), value.c_str());
        freeReplyObject(reply);
    }
    std::string Get(const std::string& key) {
        redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
        std::string value;
        if (reply->type == REDIS_REPLY_STRING) {
            value = reply->str;
        }
        freeReplyObject(reply);
        return value;
    }
    std::vector<std::string> GetAllKeys() {
        redisReply* reply = (redisReply*)redisCommand(context_, "KEYS *");
        std::vector<std::string> keys;
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; ++i) {
                keys.push_back(reply->element[i]->str);
            }
        }
        freeReplyObject(reply);
        return keys;
    }
    void Delete(const std::string& key) {
        redisReply* reply = (redisReply*)redisCommand(context_, "DEL %s", key.c_str());
        freeReplyObject(reply);
    }
};

class Session {
    public:
    Session() {
        UUID = generateUUID();
        data["is_admin"] = "1";
    }

    std::string set(std::string name) {
        if(data.find(name) == data.end()) {
            return "Error";
        }
        client.Set(UUID + name, data[name]);
        return "Success";
    }
    std::string get(std::string name) {
        if(data.find(name) == data.end()) {
            return "Error";
        }
        return client.Get(UUID + name);
    }

    private:
    const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghij";

    std::string UUID;
    RedisClient client;
    
    std::map<std::string, std::string> data;

    std::string generateUUID() {
        std::random_device rd; // Инициализируем генератор случайных чисел
        std::mt19937 gen(rd()); // Используем генератор Mersenne Twister
        std::uniform_int_distribution<int> distribution(1, 100); // Задаем диапазон случайных чисел
        std::string uuid = std::string(36,' ');
        int rnd = 0;
        int r = 0;

        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';
        
        uuid[14] = '4';

        for(int i=0;i<36;i++) {
            if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
                if (rnd <= 0x02) {
                    rnd = 0x2000000 + (distribution(gen) * 0x1000000) | 0;
                }
            rnd >>= 4;
            uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
            }
        }

        return uuid;
    }
};

int main() {
    Session sess;
    std::cout << sess.set("is_admin") << std::endl;
    std::cout << sess.get("is_admin") << std::endl;

    return 0;
}