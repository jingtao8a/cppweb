## C++11 std::chrono Introduction

1. duration
   
```cpp
template <class Rep, class Period = ratio<1> > class duration;
```
Rep是一种数值类型(int, float, double)，用来表示Period的数量
Period的类型可以为如下几种:

``` cpp
ratio<3600, 1>                hours
ratio<60, 1>                  minutes
ratio<1, 1>                   seconds
ratio<1, 1000>                miliseconds
ratio<1, 1000000>             microseconds
ratio<1, 1000000000>          nanoseconds

template <intmax_t N, intmax_t D = 1> class ratio;
```

duration还有一个成员函数count()返回Rep类型的Period数量
```cpp
// example
typedef std::chrono::duration<int, std::chrono::ratio<3600, 1>> std::chrono::hours
```

1. time_point

```cpp
template <class Clock, class Duration = typename Clock::duration>  class time_point;
```
time_point需要一个Clock，鉴于使用时间的场景不同，这个time_point具体到什么程度，由选用的单位决定。

3. Clocks
std::chrono::system_clock 它表示当前的系统时钟，系统中运行的所有进程使用now()得到的时间是一致的。每一个clock类中都有确定的time_point, duration, Rep, Period类型

## TimeStamp

```cpp
namespace CPPWEB {
    namespace clock {
        TimePoint now()   返回当前Time_point
        TimePoint nowAfter(interval)  返回interval之后Time_point
        TimePoint nowBefore(interval) 返回interval之前的Time_point
    }
}
```
