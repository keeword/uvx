#pragma once

#include "handle.hpp"

namespace uvx {

template <typename Handle>
class stream : public handle<Handle> {
public:
    int listen(CallbackWithResult callback, int backlog=128) {
        callbacks::store<internal::cid_listen>(this->get()->data, std::move(callback));

        return uv_listen(this->template get<uv_stream_t>(), backlog, [](uv_stream_t* s, int status) {
            callbacks::invoke<internal::cid_listen>(s->data, status);
        });
    }

    int accept(stream& client) {
        return uv_accept(this->template get<uv_stream_t>(), client.template get<uv_stream_t>());
    }

    int read_start(CallbackRead callback) {
        callbacks::store<internal::cid_read_start>(this->get()->data, std::move(callback));

        return uv_read_start(this->template get<uv_stream_t>(),
            [](uv_handle_t*, size_t suggested_size, uv_buf_t* buf) {
                buf->base = new char[suggested_size];
                buf->len = suggested_size;
            },
            [](uv_stream_t* s, ssize_t nread, const uv_buf_t* buf) {
				std::shared_ptr<char> holder(buf->base, std::default_delete<char[]>());

                // nread == 0 means EAGAIN or EWOULDBLOCK, not indicate error or EOF,
                // just ignore and wait for next
                if (nread != 0) {
                    callbacks::invoke<internal::cid_read_start>(s->data,
                        const_cast<const char*>(buf->base), nread);
                }
            }
        );
    }

    int read_stop() {
        return uv_read_stop(this->template get<uv_stream_t>());
    }

    bool write(const char* buf, int len, CallbackWithResult callback) {
        uv_buf_t bufs[] = { uv_buf_init(const_cast<char*>(buf), static_cast<size_t>(len)) };
        callbacks::store<internal::cid_write>(this->get()->data, std::move(callback));

        return uv_write(new uv_write_t, this->template get<uv_stream_t>(), bufs, 1,
            [](uv_write_t* req, int status) {
                std::unique_ptr<uv_write_t> holder(req);
                callbacks::invoke<internal::cid_write>(req->handle->data, status);
        });
    }

    int write(const std::string& buf, CallbackWithResult callback) {
        return write(buf.c_str(), buf.size(), std::move(callback));
    }

    int write(const std::vector<char>& buf, CallbackWithResult callback) {
        return write(buf.data(), buf.size(), std::move(callback));
    }

    int shutdown(CallbackWithResult callback) {
        callbacks::store<internal::cid_shutdown>(this->get()->data, std::move(callback));

        return uv_shutdown(new uv_shutdown_t, this->template get<uv_stream_t>(),
            [](uv_shutdown_t* req, int status) {
                std::unique_ptr<uv_shutdown_t> holder(req);
                callbacks::invoke<internal::cid_shutdown>(req->handle->data, status);
        });
    }
};

} // namespace uvx
