#include "InetAddress.hpp"
#include <strings.h>
#include <arpa/inet.h>
#include "Logger.hpp"
#include "Singleton.hpp"

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

InetAddress::InetAddress(uint16_t port, bool loopback) {
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    uint32_t ip = loopback? INADDR_LOOPBACK:INADDR_ANY;//127.0.0.1或者0.0.0.0
    m_addr.sin_addr.s_addr = htonl(ip);
    m_addr.sin_port = htons(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    int ret = ::inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr.s_addr);
    if (ret != 1) {
        SYSFATAL(logger, "InetAddress::inet_pton()");
    }
    m_addr.sin_port = htons(port);
}

std::string InetAddress::toIp() const {
    char buf[INET_ADDRSTRLEN];
    const char* ret = ::inet_ntop(AF_INET, &m_addr.sin_addr.s_addr, buf, sizeof(buf));
    if (ret == nullptr) {
        buf[0] = '\0';
        SYSERR(logger, "InetAddress::inet_ntop()");
    }
    return std::string(buf);
}

uint16_t InetAddress::toPort() const {
    return ntohs(m_addr.sin_port);
}

std::string InetAddress::toIpPort() const {
    std::string ret = toIp();
    ret.push_back(':');
    return ret.append(std::to_string(toPort()));
}


}