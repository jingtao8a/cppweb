#include "cppweb/ThreadPool.hpp"
#include <iostream>
#include "cppweb/Logger.hpp"
#include "cppweb/Singleton.hpp"

using namespace CPPWEB;

static auto& logger = Singleton<Logger>::GetInstance();

void task() {
    TRACE(logger, "hello");// %s", a++);
}

int main() {
    // ThreadPool pool(100);
    // for (size_t i = 0; i < 10; ++i) {
    //     pool.runTask(task);
    // }
    // pool.stop();
    task();
    return 0;
}