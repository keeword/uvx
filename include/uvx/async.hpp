#pragma once

#include "handle.hpp"

namespace uvx {

class async : public handle<uv_async_t> {
public:
    async(Callback callback) : handle<uv_async_t>() {
        callbacks::store<internal::cid_async>(get()->data, std::move(callback));
        uv_async_init(uv_default_loop(), get(), [](uv_async_t* h) {
            callbacks::invoke<internal::cid_async>(h->data);
        });
    }

    // send() can be called from any thread, and the callback will be invoke on the loop thread.
    // call send() multiple times before the callbake is invoked, the callback will be invoked
    // once, so call send() again after the callback was invoked.
    int send() {
        if (!is_active()) {
            throw std::runtime_error("try to call uv_async_send() on an inactive handle");
        }
        return uv_async_send(get());
    }
};

} // namespace uvx
