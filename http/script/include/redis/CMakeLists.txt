cmake_minimum_required(VERSION 3.0)

project(RedisProject)

# Добавляем подкаталог с исходными файлами модуля redis_client
add_subdirectory(redis_client)

# Главный исполняемый файл
add_executable(redis main.cpp)

# Связываем исполняемый файл с библиотекой redis_client
target_link_libraries(redis redis_client)
