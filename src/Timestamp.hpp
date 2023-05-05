#ifndef CPPWEB_TIMESTAMP_HPP
#define CPPWEB_TIMESTAMP_HPP

#include <chrono>

namespace CPPWEB {

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> TimePoint;//时间点
typedef std::chrono::nanoseconds Nanoseconds;//计量单位 纳秒

namespace clock {

inline TimePoint now() {
    return std::chrono::system_clock::now();
}

inline TimePoint nowAfter(Nanoseconds interval) {
    return now() + interval;
}

inline TimePoint nowBefore(Nanoseconds interval) {
    return now() - interval;
}

}

template<class T>
struct IntervalTypeCheckImpl {
    static constexpr bool value = 
        std::is_same<T, std::chrono::nanoseconds>::value ||
        std::is_same<T, std::chrono::microseconds>::value ||
        std::is_same<T, std::chrono::milliseconds>::value ||
        std::is_same<T, std::chrono::seconds>::value ||
        std::is_same<T, std::chrono::minutes>::value ||
        std::is_same<T, std::chrono::hours>::value;
};

#define IntervalTypeCheck(T) \
    static_assert(IntervalTypeCheck<T>::value, "bad interval type")

}


#endif