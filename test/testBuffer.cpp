#include "cppweb/Buffer.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>

int main() {
    CPPWEB::Buffer buffer1;
    int error;
    int fd = open("../test/CMakeLists.txt", O_RDONLY);
    if (fd == -1) {
        std::cout << "error";
    }
    buffer1.readFd(fd, &error);
    // auto res = buffer1.retrieveAllAsString();
    auto res = buffer1.findEOL();
    std::cout << res;//strcmp(res, "\r\n");
    // std::cout << "helo" << res  << "helo" << std::endl;
    return 0;
}