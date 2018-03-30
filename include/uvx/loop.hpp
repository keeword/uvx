#pragma once

#include <uv.h>

namespace uvx {

class loop {
public:
    loop() noexcept : loop_(uv_default_loop()) {}

    ~loop() noexcept {
        if (loop_) {
            uv_loop_close(loop_);
        }
    }

    loop(const loop&) = delete;
    loop& operator=(const loop&) = delete;
    loop(loop&& other) = delete;
    loop& operator=(loop&& other) = delete;

    // if no more active handle or request, loop will stop
    bool run() {
        return uv_run(loop_, UV_RUN_DEFAULT) == 0;
    }

    bool run_once() {
        return uv_run(loop_, UV_RUN_ONCE) == 0;
    }

    void stop() {
        uv_stop(loop_);
    }

private:
    uv_loop_t* loop_;
};

} // namespace uvx
