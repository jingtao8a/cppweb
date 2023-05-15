#ifndef CPPWEB_EPOLLER_HPP
#define CPPWEB_EPOLLER_HPP

#include "nocopyable.hpp"
#include <sys/epoll.h>
#include <vector>

namespace CPPWEB {

class EventLoop;
class Channel;

class Epoller : public nocopyable {
public:
    explicit Epoller(EventLoop* loop);
    ~Epoller();

    void poll(std::vector<Channel*>& activeChannels);
    void updateChannel(Channel* channel);

private:
    void updateChannel(int op, Channel* channel);
    EventLoop* m_loop;
    std::vector<struct epoll_event> m_events;
    int m_epollfd;
};

}

#endif