#include "session.hpp"
#include <iostream>

using namespace std;

//g++ -o test session_test.cpp session.cpp redis.cpp ../helper/helper.cpp -lhiredis

int main() {
    Session s;
    cout << s.getUUID() << endl;
    s.set(LOGIN, "user");
    cout << s.get(LOGIN) << endl;
    cout << s.IsValid() << endl;
    s.setUUID("12345678");
    cout << s.IsValid() << endl;

    return 0;
}