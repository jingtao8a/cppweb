#include "../src/Logger.hpp"
#include "../src/Singleton.hpp"

auto& logger = CPPWEB::Singleton<CPPWEB::Logger>::GetInstance();

int main() {
    logger.setLevel(CPPWEB::LOG_LEVEL_INFO);
    INFO(logger, "hello world");
    TRACE(logger, "hello world");
    DEBUG(logger, "hello world");
    return 0;
}