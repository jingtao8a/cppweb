#include "../src/EventLoopThread.hpp"
#include "../src/EventLoop.hpp"
#include "../src/Logger.hpp"
#include "../src/Singleton.hpp"
#include "../src/Timestamp.hpp"
#include <iostream>
#include <unistd.h>

auto& logger = CPPWEB::Singleton<CPPWEB::Logger>::GetInstance();

CPPWEB::EventLoop loop;


int main() {
    logger.setLevel(CPPWEB::LOG_LEVEL_TRACE);
    
    auto timer = loop.runAt(CPPWEB::clock::nowAfter(CPPWEB::Nanoseconds(4000000000)), [](){ loop.quit(); });
    auto timer2 = loop.runEvery(CPPWEB::Nanoseconds(1000000000), [](){ TRACE(logger, "hello"); });
    loop.loop();
    // CPPWEB::EventLoopThread thread;
    // auto loop = thread.startLoop();
    // auto timer = loop->runEvery(CPPWEB::Nanoseconds(1000000000), [](){ TRACE(logger, "hello"); });
    // sleep(3);
    // loop->cancelTimer(timer);
    // sleep(1);
    // loop->quit();
    return 0;
}