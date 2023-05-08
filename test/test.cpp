// #include <memory>
// #include <functional>
// #include <iostream>
// #include <stdio.h>
// class Test: public std::enable_shared_from_this<Test> {
// public:
//     Test() {
//         task = [this]() {this->test(); };
//     }
//     void test() {
//         auto ptr = shared_from_this();
//         std::cout << ptr << std::endl;
//     }
//     Test* hello() {
//         return this;
//     }
//     void runTask() {
//         if (task) {
//             task();
//         }
//     }
// private:
//     std::function<void()> task;
// };

// int main() {
//     std::shared_ptr<Test> ptr2 = std::make_shared<Test>();
//     auto& obj2 = *ptr2;
//     obj2.shared_from_this();
//     return 0;
// }

#include "../src/ThreadPool.hpp"
#include <iostream>
#include "../src/Logger.hpp"
#include "../src/Singleton.hpp"

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