#ifndef CPPWEB_LOGGER_HPP
#define CPPWEB_LOGGER_HPP

#include "nocopyable.hpp"
#define TRACE(logger, fmt, ...) do{logger.log_base(__FILE__, __LINE__, CPPWEB::LOG_LEVEL_TRACE, 0, fmt, ##__VA_ARGS__); }while(0)
#define DEBUG(logger, fmt, ...) do{logger.log_base(__FILE__, __LINE__, CPPWEB::LOG_LEVEL_DEBUG, 0, fmt, ##__VA_ARGS__); }while(0)
#define INFO(logger, fmt, ...) do{logger.log_base(__FILE__, __LINE__, CPPWEB::LOG_LEVEL_INFO, 0, fmt, ##__VA_ARGS__);  }while(0)
#define WARN(logger, fmt, ...) do{logger.log_base(__FILE__, __LINE__, CPPWEB::LOG_LEVEL_WARN, 0, fmt, ##__VA_ARGS__); }while(0)
#define ERROR(logger, fmt, ...) do{logger.log_base(__FILE__, __LINE__, CPPWEB::LOG_LEVEL_ERROR, 0, fmt, ##__VA_ARGS__);  }while(0)
#define FATAL(logger, fmt, ...) do{logger.log_base(__FILE__, __LINE__, CPPWEB::LOG_LEVEL_FATAL, 1, fmt, ##__VA_ARGS__);  }while(0)

#define SYSERR(logger, fmt, ...) do{logger.log_sys(__FILE__, __LINE__, 0, fmt, ##__VA_ARGS__); }while(0)
#define SYSFATAL(logger, fmt, ...) do{logger.log_sys(__FILE__, __LINE__, 1, fmt, ##__VA_ARGS__); }while(0)

namespace CPPWEB {

typedef enum LOG_LEVEL {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
}LOG_LEVEL;


class Logger: public nocopyable {
public:
    Logger():m_fd(0), m_level(LOG_LEVEL_TRACE) {}

    void setLevel(LOG_LEVEL level);
    void setFd(int fd);
    void log_base(const char *file, int line, 
                int level, int to_abort, const char *fmt, ...);
    void log_sys(const char *file, int line, 
                int to_abort, const char *fmt, ...);

private:
    int m_fd;
    LOG_LEVEL m_level;
};




}


#endif