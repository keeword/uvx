#include <gtest/gtest.h>

#include <thread>
#include "uvx/async.hpp"
#include "uvx/loop.hpp"

namespace {

// check active
TEST(AsyncTest, Active) {
    uvx::async async([]{});
    EXPECT_TRUE(async.is_active());
    async.close();
    EXPECT_FALSE(async.is_active());
}

// normal use case
TEST(AsyncTest, NormalCase) {
    bool run = false;
    uvx::async async([&run] {
        run = true;
    });

    async.send();

    uvx::loop loop;
    loop.run_once();

    EXPECT_TRUE(run);
}

// call from other thread
TEST(AsyncTest, Multithread) {
    bool run = false;
    uvx::loop loop;

    uvx::async async([&loop, &run] {
        run = true;
        loop.stop();
    });

    std::thread thread([&async] {
        async.send();
    });

    loop.run();

    thread.join();

    EXPECT_TRUE(run);
}

// handle called after closed
TEST(AsyncTest, SendAfterClosed) {
    bool run = false;

    uvx::async async([&run] {
        run = true;
    });

    async.close();
    EXPECT_THROW(async.send(), std::runtime_error) << "not throw exception runtime_error";

    uvx::loop loop;
    loop.run_once();

    EXPECT_FALSE(run);
}

// handle called after move
TEST(AsyncTest, SendAfterMove) {
    bool run = false;

    uvx::async async([&run] {
        run = true;
    });

    uvx::async tmp = std::move(async);

    EXPECT_THROW(async.send(), std::runtime_error) << "not throw exception runtime_error";
}

// call was coalesced
TEST(AsyncTest, CoalesceSend) {
    int count = 0;

    uvx::async async([&count] {
        count += 1;
    });

    async.send();
    async.send();
    async.send();

    uvx::loop loop;
    loop.run_once();

    EXPECT_EQ(1, count);
}

}  // namespace
