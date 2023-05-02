#ifndef CPPWEB_TIMESTAMP_HPP
#define CPPWEB_TIMESTAMP_HPP

#include <chrono>

namespace CPPWEB {

namespace clock {

inline std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now() {
    return std::chrono::system_clock::now();
}

inline std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> nowAfter(std::chrono::nanoseconds interval) {
    return now() + interval;
}

inline std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> nowBefore(std::chrono::nanoseconds interval) {
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