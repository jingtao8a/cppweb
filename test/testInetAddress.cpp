#include "cppweb/InetAddress.hpp"
#include <iostream>

int main() {
    CPPWEB::InetAddress addr1("192.123.4.1", 1000);
    std::cout << addr1.toIpPort();
    return 0;
}