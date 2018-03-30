#pragma once

#include <stdexcept>
#include "callback.hpp"

namespace uvx {

template <typename Handle>
class handle {
protected:
    handle()
        : handle_(std::make_unique<Handle>()), 
          callback_(std::make_unique<callbacks>()) {
        uv_handle_set_data(get<uv_handle_t>(), reinterpret_cast<void*>(callback_.get()));
    }

    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;

    handle(handle&& other) noexcept
        : handle_(std::move(other.handle_)),
          callback_(std::move(other.callback_)) {}

    handle& operator=(handle&& other) {
        if (this != &other) {
            close();
            handle_ = std::move(other.handle_);
            callback_ = std::move(other.callback_);
        }
        return *this;
    }

    virtual ~handle() {
        close();
    }

    template <typename T=Handle>
    T* get() {
        if (!handle_) {
            throw std::runtime_error("handle not exist");
        }
        return reinterpret_cast<T*>(handle_.get());
    }

    template <typename T=Handle>
    const T* get() const {
        if (!handle_) {
            throw std::runtime_error("handle not exist");
        }
        return reinterpret_cast<const T*>(handle_.get());
    }

public:
    // is closing or closed?
    bool is_closing() const {
        if (handle_) {
            return uv_is_closing(get<uv_handle_t>()) != 0;
        }
        return true;
    }

    bool is_active() const {
        if (!is_closing()) {
            return uv_is_active(get<uv_handle_t>()) != 0;
        }
        return false;
    }

    void close(Callback callback = [] {}) {
        if (is_closing()) {
            return;
        }

        callbacks::store<internal::cid_close>(handle_->data, std::move(callback));
        uv_close(get<uv_handle_t>(), [](uv_handle_t* h) {
            std::unique_ptr<Handle> handle_holder(reinterpret_cast<Handle*>(h));
            std::unique_ptr<callbacks> callback_holder(reinterpret_cast<callbacks*>(h->data));
            callbacks::invoke<internal::cid_close>(h->data);
        });

        // in some case, the handle object will destructed before the callback invoked,
        // so we need transfer ownership to libuv, and free it in the close callback
        callback_.release();
        handle_.release();
    }

private:
    std::unique_ptr<callbacks> callback_;
    std::unique_ptr<Handle> handle_;
};

} // namespace uvx
