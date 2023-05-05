#include "../src/EventLoop.hpp"
#include "../src/Logger.hpp"
#include "../src/Singleton.hpp"
#include "../src/Timestamp.hpp"
#include <iostream>

auto& logger = CPPWEB::Singleton<CPPWEB::Logger>::GetInstance();

int main() {
    logger.setLevel(CPPWEB::LOG_LEVEL_TRACE);
    CPPWEB::EventLoop loop;
    CPPWEB::Timer *timer = loop.runAt(CPPWEB::clock::nowAfter(CPPWEB::Nanoseconds(1000000000)), [](){ TRACE(logger, "hello"); });
    loop.cancelTimer(timer);
    loop.loop();
    return 0;
}