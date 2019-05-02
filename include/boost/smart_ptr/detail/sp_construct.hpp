/*
Copyright 2012-2019 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef BOOST_SMART_PTR_DETAIL_SP_CONSTRUCT_HPP
#define BOOST_SMART_PTR_DETAIL_SP_CONSTRUCT_HPP

#include <boost/core/noinit_adaptor.hpp>

namespace boost {
namespace detail {

#if !defined(BOOST_NO_CXX11_ALLOCATOR)
template<class A, class T>
inline void
sp_destroy(A& a, T* p)
{
    std::allocator_traits<A>::destroy(a, p);
}

template<class A, class T>
inline void
sp_destroy_n(A& a, T* p, std::size_t n)
{
    while (n > 0) {
        std::allocator_traits<A>::destroy(a, p + --n);
    }
}
#else
template<class A, class T>
inline void
sp_destroy(A&, T* p)
{
    p->~T();
}

template<class A, class T>
inline void
sp_destroy_n(A&, T* p, std::size_t n)
{
    while (n > 0) {
        p[--n].~T();
    }
}
#endif

template<class A, class T>
class sp_destroyer {
public:
    sp_destroyer(A& a, T* p) BOOST_SP_NOEXCEPT
        : a_(a),
          p_(p),
          n_(0) { }

    ~sp_destroyer() {
        sp_destroy_n(a_, p_, n_);
    }

    std::size_t& size() BOOST_SP_NOEXCEPT {
        return n_;
    }

private:
    sp_destroyer(const sp_destroyer&);
    sp_destroyer& operator=(const sp_destroyer&);

    A& a_;
    T* p_;
    std::size_t n_;
};

#if !defined(BOOST_NO_CXX11_ALLOCATOR)
template<class A, class T>
inline void
sp_construct(A& a, T* p)
{
    std::allocator_traits<A>::construct(a, p);
}

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
template<class A, class T, class U, class... V>
inline void
sp_construct(A& a, T* p, U&& u, V&&... v)
{
    std::allocator_traits<A>::construct(a, p, std::forward<U>(u),
        std::forward<V>(v)...);
}
#else
template<class A, class T, class U>
inline void
sp_construct(A& a, T* p, U&& u)
{
    std::allocator_traits<A>::construct(a, p, std::forward<U>(u));
}
#endif
#else
template<class A, class T, class U>
inline void
sp_construct(A& a, T* p, const U& u)
{
    std::allocator_traits<A>::construct(a, p, u);
}
#endif

template<class A, class T>
inline void
sp_construct_n(A& a, T* p, std::size_t n)
{
    sp_destroyer<A, T> hold(a, p);
    for (std::size_t& i = hold.size(); i < n; ++i) {
        std::allocator_traits<A>::construct(a, p + i);
    }
    hold.size() = 0;
}

template<class A, class T>
inline void
sp_construct_n(A& a, T* p, std::size_t n, const T* l, std::size_t m)
{
    sp_destroyer<A, T> hold(a, p);
    for (std::size_t& i = hold.size(); i < n; ++i) {
        std::allocator_traits<A>::construct(a, p + i, l[i % m]);
    }
    hold.size() = 0;
}
#else
template<class A, class T>
inline void
sp_construct(A&, T* p)
{
    ::new(static_cast<void*>(p)) T();
}

template<class A, class T>
inline void
sp_construct(boost::noinit_adaptor<A>&, T* p)
{
    ::new(static_cast<void*>(p)) T;
}

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
template<class A, class T, class U, class... V>
inline void
sp_construct(A&, T* p, U&& u, V&&... v)
{
    ::new(static_cast<void*>(p)) T(std::forward<U>(u), std::forward<V>(v)...);
}
#else
template<class A, class T, class U>
inline void
sp_construct(A&, T* p, U&& u)
{
    ::new(static_cast<void*>(p)) T(std::forward<U>(u));
}
#endif
#else
template<class A, class T, class U>
inline void
sp_construct(A&, T* p, const U& u)
{
    ::new(static_cast<void*>(p)) T(u);
}
#endif

template<class A, class T>
inline void
sp_construct_n(A& a, T* p, std::size_t n)
{
    sp_destroyer<A, T> hold(a, p);
    for (std::size_t& i = hold.size(); i < n; ++i) {
        ::new(static_cast<void*>(p + i)) T();
    }
    hold.size() = 0;
}

template<class A, class T>
inline void
sp_construct_n(boost::noinit_adaptor<A>& a, T* p, std::size_t n)
{
    sp_destroyer<boost::noinit_adaptor<A>, T> hold(a, p);
    for (std::size_t& i = hold.size(); i < n; ++i) {
        ::new(static_cast<void*>(p + i)) T;
    }
    hold.size() = 0;
}

template<class A, class T>
inline void
sp_construct_n(A& a, T* p, std::size_t n, const T* l, std::size_t m)
{
    sp_destroyer<A, T> hold(a, p);
    for (std::size_t& i = hold.size(); i < n; ++i) {
        ::new(static_cast<void*>(p + i)) T(l[i % m]);
    }
    hold.size() = 0;
}
#endif

} /* detail */
} /* boost */

#endif
