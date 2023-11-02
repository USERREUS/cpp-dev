# Создаем библиотеку redis_client
add_library(redis_client redis_client.cpp)

# Добавляем путь к заголовочным файлам для этой библиотеки
target_include_directories(redis_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Связываем библиотеку hiredis с нашей библиотекой redis_client
target_link_libraries(redis_client hiredis)
