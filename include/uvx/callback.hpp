#pragma once

#include <uv.h>
#include "method.hpp"

namespace uvx {

typedef std::function<void()> Callback;
typedef std::function<void(int)> CallbackWithResult;
typedef std::function<void(const char*, ssize_t)> CallbackRead;
typedef std::function<void(const char*, ssize_t, const std::string, int)> CallbackUdpRecv;

namespace internal {
enum callback_id {
	// Callback
	placeholder_cid_1 = 0,
	cid_async,
	cid_close,
	cid_fs_close,
    cid_timer,
	cid_work,

	// CallbackWithResult
	placeholder_cid_2,
    cid_after_work,
    cid_connect,
    cid_connect6,
    cid_listen,
	cid_fs_open,
	cid_fs_write,
	cid_fs_rename,
    cid_shutdown,
	cid_udp_send,
	cid_write,

	// CallbackRead
	placeholder_cid_3,
	cid_fs_read,
    cid_read_start,

	// CallbackUdpRecv
	placeholder_cid_4,
    cid_udp_recv,

	cid_max
};

template <bool> struct range;

template <size_t val, typename = range<true> >
struct traits {};

template <size_t val>
struct traits<val, range<(placeholder_cid_1 <= val && val < placeholder_cid_2)> > {
    using argument_tuple = functype::traits<Callback>::argument_tuple;
};

template <size_t val>
struct traits<val, range<(placeholder_cid_2 <= val && val < placeholder_cid_3)> > {
    using argument_tuple = functype::traits<CallbackWithResult>::argument_tuple;
};

template <size_t val>
struct traits<val, range<(placeholder_cid_3 <= val && val < placeholder_cid_4)> > {
    using argument_tuple = functype::traits<CallbackRead>::argument_tuple;
};

template <size_t val>
struct traits<val, range<(placeholder_cid_4 <= val && val < cid_max)> > {
    using argument_tuple = functype::traits<CallbackUdpRecv>::argument_tuple;
};

} // namespace internal

class callbacks {
	using callback_type = std::function<void(std::any)>;
public:
    template <internal::callback_id Cid, typename Callback>
    static void store(void* target, Callback callback) {
		static_assert(std::is_void<typename functype::traits<Callback>::result_type>::value,
			"callback must return void");

        static_assert(std::is_same<typename internal::traits<Cid>::argument_tuple,
            typename functype::traits<Callback>::argument_tuple>::value,
			"the argument type of callback must match that requirement of cid");

        reinterpret_cast<callbacks*>(target)->method_.store(Cid, std::move(callback));
    }

    template <internal::callback_id Cid, typename... Args>
    static void invoke(void* target, Args&&... args) {
        static_assert(std::is_same<typename internal::traits<Cid>::argument_tuple,
			std::tuple<typename std::remove_reference<Args>::type...>>::value,
			"the argument type must match that requirement of cid");

        reinterpret_cast<callbacks*>(target)->method_.invoke(Cid, std::forward<Args>(args)...);
    }

private:
	utils::methods method_;
};

} // namespace uvx
