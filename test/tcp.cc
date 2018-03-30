#include <gtest/gtest.h>

#include <thread>
#include "uvx/tcp.hpp"
#include "uvx/loop.hpp"

#define TEST_ADDR "127.0.0.1"
#define TEST_PORT 10080
#define TEST_STRING "PING"

namespace {

// normal case
TEST(TcpTest, NormalCase) {
    uvx::tcp server;
    uvx::tcp session;
    uvx::tcp client;

    server.bind(TEST_ADDR, TEST_PORT);
    server.listen([&server, &session](int) {
        server.accept(session);
        session.read_start([&session, &server](const char* data, ssize_t) {
            EXPECT_STREQ(TEST_STRING, data);

            bool ip4; std::string ip; int port;
            EXPECT_TRUE(server.getsockname(ip4, ip, port));
            EXPECT_STREQ(ip.c_str(), TEST_ADDR);
            EXPECT_EQ(TEST_PORT, port);
            EXPECT_TRUE(ip4);

            EXPECT_EQ(0, session.read_stop());

            session.close();
            server.close();
        });
    });

    client.connect(TEST_ADDR, TEST_PORT, [&client](int) {
        std::string str(TEST_STRING);
        client.write(str.c_str(), str.size(), [&client](int) {
            bool ip4; std::string ip; int port;
            EXPECT_TRUE(client.getpeername(ip4, ip, port));
            EXPECT_STREQ(ip.c_str(), TEST_ADDR);
            EXPECT_EQ(TEST_PORT, port);
            EXPECT_TRUE(ip4);

            client.close();
        });
    });

    uvx::loop loop;
    loop.run();
}

// you can bind the same port multiple times,
// but only one of them can listen or connect successfully
TEST(TcpTest, BindInUsed) {
    uvx::tcp server;
    uvx::tcp session;

    EXPECT_TRUE(0 == server.bind(TEST_ADDR, TEST_PORT));
    EXPECT_TRUE(0 == session.bind(TEST_ADDR, TEST_PORT));

    EXPECT_TRUE(0 == server.listen([](int) {}));
    EXPECT_FALSE(0 == session.listen([](int) {}));
}

// get connection infomation before it is established
TEST(TcpTest, GetInfoBeforeConnected) {
    uvx::tcp server;

    bool ip4 = true;
    std::string ip{TEST_ADDR};
    int port{TEST_PORT};
    EXPECT_FALSE(server.getsockname(ip4, ip, port));
    EXPECT_FALSE(server.getpeername(ip4, ip, port));
    EXPECT_STREQ(ip.c_str(), TEST_ADDR);
    EXPECT_EQ(TEST_PORT, port);
    EXPECT_TRUE(ip4);
}

}  // namespace
