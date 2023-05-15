#ifndef CPPWEB_SINGLETON_HPP
#define CPPWEB_SINGLETON_HPP

namespace CPPWEB {

template<class T>
class Singleton {
public:
    static T& GetInstance() {
        static T v;
        return v;
    }
};

}
#endif