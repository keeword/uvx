#pragma once

#include <stdexcept>
#include <memory>
#include "callback.hpp"

namespace uvx {

template <typename Request>
class request {
protected:
    request()
		: request_(std::make_unique<Request>()),
		  callback_(std::make_unique<callbacks>()) {
		uv_req_set_data(get<uv_req_t>(), reinterpret_cast<void*>(callback_.get()));
    }

    request(const request&) = delete;
    request& operator=(const request&) = delete;

    request(request&& other)
		: request_(std::move(other.request_)),
	      callback_(std::move(other.callback_)) {}

    request& operator=(request&& other) {
        if (this != &other) {
			request_ = std::move(other.request_);
			callback_ = std::move(other.callback_);
		}
        return *this;
    }

	~request() {}

    template <typename T=Request>
    T* get() {
        if (!request_) {
            throw std::runtime_error("request not exist");
        }
        return reinterpret_cast<T*>(request_.get());
    }

    template <typename T=Request>
    const T* get() const {
        if (!request_) {
            throw std::runtime_error("request not exist");
        }
        return reinterpret_cast<const T*>(request_.get());
    }

public:
    int cancel() {
        return uv_cancel(get<uv_req_t>());
    }

private:
    std::unique_ptr<Request> request_;
	std::unique_ptr<callbacks> callback_;
};

} // namespace
