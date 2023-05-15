#ifndef CPPWEB_NOCOPYABLE_HPP
#define CPPWEB_NOCOPYABLE_HPP

namespace CPPWEB {
class nocopyable {
public:
    nocopyable(const nocopyable&) = delete;
    nocopyable& operator=(const nocopyable&) = delete;

protected:
    nocopyable() = default;
    ~nocopyable() = default;
};

}

#endif