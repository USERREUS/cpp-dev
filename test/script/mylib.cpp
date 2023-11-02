#include <iostream>
#include "mylib.hpp"

void Test::_say() {
    std::cout << "Hello prvt!" << std::endl;
}

void Test::say() {
    std::cout << "Hello public!" << std::endl;
}
