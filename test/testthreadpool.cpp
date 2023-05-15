#include "cppweb/ThreadPool.hpp"
#include "cppweb/Logger.hpp"
#include "cppweb/Singleton.hpp"
#include <iostream>
#include <atomic>
#include <unistd.h>

auto& logger = CPPWEB::Singleton<CPPWEB::Logger>::GetInstance();

std::atomic<int> a(0);

void func() {
    std::cout << "hello " << a++ << std::endl;
}

int main() {
    logger.setLevel(CPPWEB::LOG_LEVEL_TRACE);
    CPPWEB::ThreadPool pool(10);
    for (int i = 0; i < 40; ++i)
        pool.runTask(func);
    return 0;
}
