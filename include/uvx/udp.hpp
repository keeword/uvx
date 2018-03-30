#pragma once

#include "stream.hpp"
#include "net.hpp"

#include <cassert>
#include <string>

namespace uvx {

class udp : public stream<uv_udp_t> {
public:
    enum udp_membership { LEAVE_GROUP = UV_LEAVE_GROUP, JOIN_GROUP = UV_JOIN_GROUP };

    udp() {
        uv_udp_init_ex(uv_default_loop(), get(), AF_INET);
    }

    int bind(const std::string& ip, int port) {
        ip4_addr addr = to_ip4_addr(ip, port);
        return uv_udp_bind(get(), reinterpret_cast<sockaddr*>(&addr), 0);
    }

    int bind6(const std::string& ip, int port) {
        ip6_addr addr = to_ip6_addr(ip, port);
        return uv_udp_bind(get(), reinterpret_cast<sockaddr*>(&addr), 0);
    }

    bool getsockname(bool& ip4, std::string& ip, int& port) {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (uv_udp_getsockname(get(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0) {
            ip4 = (addr.ss_family == AF_INET);
            if (ip4) {
                return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
            } else {
                return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
            }
        }
        return false;
    }

    int set_membership(const std::string& ip, const std::string& interfaces, udp_membership membership) {
        return uv_udp_set_membership(get(), ip.c_str(), interfaces.c_str(), static_cast<uv_membership>(membership));
    }

    int set_multicast_ttl(int ttl) {
        assert(ttl >= 1 && ttl <= 255);
        return uv_udp_set_multicast_ttl(get(), ttl);
    }

    int set_multicast_interface(const std::string& interfaces) {
        return uv_udp_set_multicast_interface(get(), interfaces.c_str());
    }

    int set_broadcast(bool on) {
        return uv_udp_set_broadcast(get(), on ? 1 : 0);
    }

    int set_ttl(int ttl) {
        assert(ttl >= 1 && ttl <= 255);
        return uv_udp_set_ttl(get(), ttl);
    }

    int send(const char* buf, size_t len, const std::string& ip, int port, CallbackWithResult callback) {
        uv_buf_t bufs[] = { uv_buf_init(const_cast<char*>(buf), static_cast<size_t>(len)) };
        ip4_addr addr = to_ip4_addr(ip, port);

        callbacks::store<internal::cid_udp_send>(this->get()->data, std::move(callback));

        return uv_udp_send(new uv_udp_send_t, get(), bufs, 1, reinterpret_cast<struct sockaddr*>(&addr),
            [](uv_udp_send_t* req, int status) {
                std::unique_ptr<uv_udp_send_t> holder(req);
                callbacks::invoke<internal::cid_udp_send>(req->handle->data, status);
        });
    }

    int recv_start(CallbackUdpRecv callback) {
        callbacks::store<internal::cid_udp_recv>(this->get()->data,  std::move(callback));

        return uv_udp_recv_start(get(),
            [](uv_handle_t*, size_t suggested_size, uv_buf_t* buf) {
                buf->base = new char[suggested_size];
                buf->len = suggested_size;
            },
            [](uv_udp_t* h, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
                std::shared_ptr<char> holder(buf->base, std::default_delete<char[]>());
                // nread < 0 means error, nread == 0 means nothing to read or receive an empty packet
                // ignore error and nothing to read
                if (nread >= 0 && addr) {
                    std::string ip; int port;
                    if (addr->sa_family == AF_INET) {
                        from_ip4_addr(reinterpret_cast<const ip4_addr*>(addr), ip, port);
                    } else if (addr->sa_family == AF_INET6) {
                        from_ip6_addr(reinterpret_cast<const ip6_addr*>(addr), ip, port);
                    } else {
                        return;
                    }
                    callbacks::invoke<internal::cid_udp_recv>(h->data,
                        const_cast<const char*>(buf->base), nread, ip, port);
                }
        });
    }

    int recv_stop() {
        return uv_udp_recv_stop(get());
    }
};

} // namespace uvx
