#pragma once

#include <tuple>
#include <functional>

namespace functype {

template <typename T>
struct traits : public traits<decltype(&T::operator())> {};

template <typename R, typename... A>
struct traits<R(A...)> {
	typedef R result_type;
    typedef std::tuple<A...> argument_tuple;
};

template <typename R, typename... A>
struct traits<R(*)(A...)> : public traits<R(A...)> {};

template <typename R, typename C, typename... A>
struct traits<R(C::*)(A...)> : public traits<R(A...)> {};

template <typename R, typename C, typename... A>
struct traits<R(C::*)(A...) const> : public traits<R(A...)> {};

template <typename R, typename C, typename... A>
struct traits<R(C::*)(A...) volatile> : public traits<R(A...)> {};

template <typename R, typename C, typename... A>
struct traits<R(C::*)(A...) const volatile> : public traits<R(A...)> {};

template <typename T>
struct traits<std::function<T>> : public traits<T> {};

template <typename T>
struct traits<T&> : public traits<T> {};

template <typename T>
struct traits<const T&> : public traits<T> {};

template <typename T>
struct traits<volatile T&> : public traits<T> {};

template <typename T>
struct traits<const volatile T&> : public traits<T> {};

template <typename T>
struct traits<T&&> : public traits<T> {};

template <typename T>
struct traits<const T&&> : public traits<T> {};

template <typename T>
struct traits<volatile T&&> : public traits<T> {};

template <typename T>
struct traits<const volatile T&&> : public traits<T> {};

} // namespace functype
