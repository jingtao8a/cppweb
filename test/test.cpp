#include <memory>
#include <functional>
#include <iostream>
#include <stdio.h>
class Test: public std::enable_shared_from_this<Test> {
public:
    Test() {
        task = [this]() {this->test(); };
    }
    void test() {
        auto ptr = shared_from_this();
        std::cout << ptr << std::endl;
    }
    Test* hello() {
        return this;
    }
    void runTask() {
        if (task) {
            task();
        }
    }
private:
    std::function<void()> task;
};

int main() {
    std::shared_ptr<Test> ptr2 = std::make_shared<Test>();
    auto& obj2 = *ptr2;
    obj2.shared_from_this();
    return 0;
}