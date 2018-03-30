#pragma once

#include "stream.hpp"
#include "net.hpp"

namespace uvx {

class tcp : public stream<uv_tcp_t> {
public:
    tcp() {
        uv_tcp_init(uv_default_loop(), get());
    }

    int nodelay(bool enable) {
        return uv_tcp_nodelay(get(), enable ? 1 : 0);
    }

    int keepalive(bool enable, unsigned int delay) {
        return uv_tcp_keepalive(get(), enable ? 1 : 0, delay);
    }

    int simultanious_accepts(bool enable) {
        return uv_tcp_simultaneous_accepts(get(), enable ? 1 : 0);
    }

    int bind(const std::string& ip, int port) {
        ip4_addr addr = to_ip4_addr(ip, port);
        return uv_tcp_bind(get(), reinterpret_cast<sockaddr*>(&addr), 0);
    }

    int bind6(const std::string& ip, int port) {
        ip6_addr addr = to_ip6_addr(ip, port);
        return uv_tcp_bind(get(), reinterpret_cast<sockaddr*>(&addr), 0);
    }

    int connect(const std::string& ip, int port, CallbackWithResult callback) {
        callbacks::store<internal::cid_connect>(get()->data, std::move(callback));

        ip4_addr addr = to_ip4_addr(ip, port);
        return uv_tcp_connect(new uv_connect_t, get(), reinterpret_cast<const sockaddr*>(&addr),
            [](uv_connect_t* req, int status) {
                std::unique_ptr<uv_connect_t> holder{ req };
                callbacks::invoke<internal::cid_connect>(req->handle->data, status);
        });
    }

    int connect6(const std::string& ip, int port, CallbackWithResult callback) {
        callbacks::store<internal::cid_connect6>(get()->data, std::move(callback));

        ip6_addr addr = to_ip6_addr(ip, port);
        return uv_tcp_connect(new uv_connect_t, get(), reinterpret_cast<const sockaddr*>(&addr),
            [](uv_connect_t* req, int status) {
                std::unique_ptr<uv_connect_t> holder{ req };
                callbacks::invoke<internal::cid_connect6>(req->handle->data, status);
        });
    }

    bool getsockname(bool& ip4, std::string& ip, int& port) const {
        sockaddr_storage addr;
        int len = sizeof(addr);
        if (uv_tcp_getsockname(get(), reinterpret_cast<sockaddr*>(&addr), &len) == 0) {
            ip4 = (addr.ss_family == AF_INET);
            if (ip4) {
                return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
            } else {
                return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
            }
        }
        return false;
    }

    bool getpeername(bool& ip4, std::string& ip, int& port) const {
        sockaddr_storage addr;
        int len = sizeof(addr);
        if (uv_tcp_getpeername(get(), reinterpret_cast<sockaddr*>(&addr), &len) == 0) {
            ip4 = (addr.ss_family == AF_INET);
            if (ip4) {
                return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
            } else {
                return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
            }
        }
        return false;
    }
};

} // namespace uvx
