#ifndef __SERIALIZATION_HELPERS_H_INCLUDED__
#define __SERIALIZATION_HELPERS_H_INCLUDED__

namespace SerializationHelpers{

template<bool C, class T1, class T2>
struct Selector{};

template<class T1, class T2>
struct Selector<false, T1, T2>{
    typedef T2 type;
};

template<class T1, class T2>
struct Selector<true, T1, T2>{
    typedef T1 type;
};

template<class C, class T1, class T2>
struct Select{
    typedef typename Selector<C::value, T1,T2>::type selected_type;
    typedef typename selected_type::type type;
};

template<class T>
struct Identity{
    typedef T type;
};

template<class T>
struct IsClass{
private:
    struct NoType { char dummy; };
    struct YesType { char dummy[100]; };

    template<class U>
    static YesType function_helper(void(U::*)(void));

    template<class U>
    static NoType function_helper(...);
public:
    enum{ value = (sizeof(function_helper<T>(0)) == sizeof(YesType))};
};

};

#endif
