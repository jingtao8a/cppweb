#ifndef CPPWEB_TIMER_HPP
#define CPPWEB_TIMER_HPP

#include <functional>
#include <assert.h>
#include "Timestamp.hpp"
#include "nocopyable.hpp"

namespace CPPWEB {
class Timer: public nocopyable {
public:
    Timer(std::function<void()> callback, TimePoint when, Nanoseconds interval) :
        m_callback(callback),
        m_when(when),
        m_interval(interval),
        m_repeat(m_interval > Nanoseconds::zero()),
        m_canceled(false) {}

    bool isRepeat() const { return m_repeat; } 
    bool isCanceled() const { return m_canceled; }
    TimePoint when() const { return m_when; }
    bool expired(TimePoint now) const { return now >= m_when; }
    
    void cancel() {
        assert(!m_canceled);//未停止
        m_canceled = true;
    }
    
    void restart() {
        assert(m_repeat);//被设置为可以重复
        m_when += m_interval;
    }
    
    void run() {
        if (m_callback) {
            m_callback();
        }
    }
private:
    std::function<void()> m_callback;
    TimePoint m_when;
    const Nanoseconds m_interval;
    bool m_repeat;
    bool m_canceled;
}; 

}

#endif