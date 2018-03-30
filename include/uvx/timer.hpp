#pragma once

#include <chrono>

#include "handle.hpp"

namespace uvx {

class timer : public handle<uv_timer_t> {
    using milliseconds = std::chrono::duration<uint64_t, std::milli>;
public:
    timer() {
        uv_timer_init(uv_default_loop(), get());
    }

    int start(Callback callback, milliseconds timeout, milliseconds repeat) {
        callbacks::store<internal::cid_timer>(get()->data, std::move(callback));

        return uv_timer_start(get(),
            [](uv_timer_t* h) {
                callbacks::invoke<internal::cid_timer>(h->data); },
            timeout.count(),
            repeat.count());
    }

    int start(Callback callback, milliseconds timeout) {
        return start(std::move(callback), timeout, milliseconds(0));
    }

    int stop() {
        return uv_timer_stop(get());
    }

    int again() {
        return uv_timer_again(get());
    }
};

} // namespace uvx
