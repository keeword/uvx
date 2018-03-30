#pragma once

#include "request.hpp"

namespace uvx {

class work : public request<uv_work_t> {
public:
    int execute(Callback callback, CallbackWithResult afterCallback) {
        callbacks::store<internal::cid_work>(get()->data, std::move(callback));
        callbacks::store<internal::cid_after_work>(get()->data, std::move(afterCallback));

        return uv_queue_work(uv_default_loop(), get(),
            [](uv_work_t* req) {
                // run on the thread pool
                callbacks::invoke<internal::cid_work>(req->data);
            },
            [](uv_work_t* req, int status) {
                // run on the loop thread
                callbacks::invoke<internal::cid_after_work>(req->data, status);
        });
    }
};

} // namespace uvx
