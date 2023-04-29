#include "Logger.hpp"
#include <time.h>
#include <sys/time.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#define MAXLINE 256

namespace CPPWEB {
static const char *log_level_str[] = {
        "[TRACE]",
        "[DEBUG]",
        "[INFO] ",
        "[WARN] ",
        "[ERROR]",
        "[FATAL]"
};

static int timestamp(char *data, size_t len) {//获取精确的UTC时间
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t seconds = tv.tv_sec;

    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    
    return snprintf(data, len, "%4d%02d%02d %02d:%02d:%02d.%06ld",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, tv.tv_usec);
}

void Logger::log_base(const char *file, int line, 
                int level, int to_abort, const char *fmt, ...){
    if (m_level < level) {
        return;
    }
    char data[MAXLINE];
    size_t i = 0;
    va_list ap;
    i += timestamp(data, 32);//往data中填充时间信息
    i += sprintf(data + i, " [%d]", getpid());//当前进程号
    i += sprintf(data + i, " %s ", log_level_str[level]);

    va_start(ap, fmt);
    vsnprintf(data + i, MAXLINE - i, fmt, ap);
    va_end(ap);
    int err = dprintf(m_fd, "%s - %s:%d\n", data, strrchr(file, '/') + 1, line);
    if (err == -1) {
        fprintf(stderr, "log failed");
    }

    if (to_abort) {
        exit(-1);
    }
}

void Logger::log_sys(const char *file, int line, 
                int to_abort, const char *fmt, ...) {
    char data[MAXLINE];
    size_t i = 0;
    va_list ap;

    i += timestamp(data, 32);
    i += sprintf(data + i, " [%d]", getpid());
    i += sprintf(data + i, " %s ", to_abort ? "[SYSFA]":"[SYSER]");

    va_start(ap, fmt);
    vsnprintf(data + i, MAXLINE - i, fmt, ap);
    va_end(ap);

    dprintf(m_fd, "%s: %s - %s:%d\n",
            data, strerror(errno), strrchr(file, '/') + 1, line);

    if (to_abort) {
        exit(-1);
    }
}

void Logger::setFd(int fd) {
    if (fd < 0) {
        fd = 0;
    }
    m_fd = fd;
}
void Logger::setLevel(LOG_LEVEL level) {
    if (level < LOG_LEVEL_TRACE) {
        level = LOG_LEVEL_TRACE;
    }
    if (level > LOG_LEVEL_FATAL) {
        level = LOG_LEVEL_FATAL;
    }
    m_level = level;
}

}