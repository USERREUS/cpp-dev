#include "redis_client.hpp"

int main() {
    RedisClient redis("127.0.0.1", 6379);

    redis.Set("mykey", "Hello");

    std::string value = redis.Get("mykey");
    std::cout << "Значение mykey: " << value << std::endl;

    std::vector<std::string> keys = redis.GetAllKeys();
    for (const std::string& key : keys) {
        std::cout << "Ключ: " << key << std::endl;
    }

    redis.Delete("mykey");

    return 0;
}