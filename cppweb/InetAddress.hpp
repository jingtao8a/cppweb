#ifndef CPPWEB_INETADDRESS_HPP
#define CPPWEB_INETADDRESS_HPP

#include <string>
#include <netinet/in.h>

namespace CPPWEB {

class InetAddress {
public:
    explicit InetAddress(uint16_t port=0, bool loopback=false);
    InetAddress(const std::string& ip, uint16_t port);

    void setAddress(const struct sockaddr_in& addr) { m_addr = addr; }
    const struct sockaddr* getSockaddr() const { 
        return reinterpret_cast<const struct sockaddr*>(&m_addr);}
    socklen_t getSocklen() const { 
        return sizeof(m_addr); }

    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;

private:
    struct sockaddr_in m_addr;
};

}

#endif