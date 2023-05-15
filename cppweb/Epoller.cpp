#include "Epoller.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include "EventLoop.hpp"
namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

Epoller::Epoller(EventLoop* loop):
    m_loop(loop),
    m_events(128),
    m_epollfd(::epoll_create1(EPOLL_CLOEXEC)) {//和O_CLOEXEC的作用类似，避免描述符被继承到子进程中

    if (m_epollfd == -1) {
        SYSFATAL(logger, "Epoller::epoll_create1()");
    }
}

Epoller::~Epoller() {
    ::close(m_epollfd);
}

void Epoller::poll(std::vector<Channel *>& activeChannels) {
    m_loop->assertInLoopThread();
    int maxEvents = static_cast<int>(m_events.size());
    int nEvents = ::epoll_wait(m_epollfd, m_events.data(), maxEvents, -1);
    if (nEvents == -1) {
        if (errno != EINTR) {
            SYSERR(logger, "Epoller::epoll_wait()");
        }
    } else if (nEvents > 0) {
        for (int i = 0; i < nEvents; ++i) {
            auto channel = static_cast<Channel*>(m_events[i].data.ptr);
            channel->setRevents(m_events[i].events);//保存触发的事件
            activeChannels.push_back(channel);
        }
        if (nEvents == maxEvents) {
            m_events.resize(2 * m_events.size());
        }
    }
}

void Epoller::updateChannel(Channel* channel) {
    m_loop->assertInLoopThread();
    int op = 0;
    if (!channel->polling) {
        assert(!channel->isNoneEvents());
        op = EPOLL_CTL_ADD;
        channel->polling = true;
    } else if (channel->isNoneEvents()){
        op = EPOLL_CTL_DEL;
        channel->polling = false;
    } else {
        op = EPOLL_CTL_MOD;
    }
    updateChannel(op, channel);
}

void Epoller::updateChannel(int op, Channel* channel) {
    struct epoll_event e;
    e.events = channel->getEvents();
    e.data.ptr = channel;
    int ret = ::epoll_ctl(m_epollfd, op, channel->getFd(), &e);
    if (ret == -1) {
        SYSERR(logger, "Epoller::epoll_ctl()");
    }
}

}