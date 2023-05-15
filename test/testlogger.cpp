#include "cppweb/Logger.hpp"
#include "cppweb/Singleton.hpp"

static auto& logger = CPPWEB::Singleton<CPPWEB::Logger>::GetInstance();

int main() {
    logger.setLevel(CPPWEB::LOG_LEVEL_INFO);
    INFO(logger, "hello world");
    TRACE(logger, "hello world");
    DEBUG(logger, "hello world");
    return 0;
}