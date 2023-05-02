#ifndef CPPWEB_TIMER_HPP
#define CPPWEB_TIMER_HPP

#include <functional>
#include <assert.h>
#include "Timestamp.hpp"
#include "nocopyable.hpp"

namespace CPPWEB {
class Timer: public nocopyable {
public:
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> TimePoint;
    typedef std::chrono::nanoseconds Nanseconds;
    
    Timer(std::function<void()> callback, TimePoint when, Nanseconds interval) :
        m_callback(callback),
        m_when(when),
        m_interval(interval),
        m_repeat(m_interval > Nanseconds::zero()),
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
    const Nanseconds m_interval;
    bool m_repeat;
    bool m_canceled;
}; 

}

#endif