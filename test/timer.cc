#include <gtest/gtest.h>

#include "uvx/timer.hpp"
#include "uvx/loop.hpp"

namespace {

// timeout use case
TEST(TimerTest, Timeout) {
    int count = 0;
    uvx::timer timer;
    uvx::loop loop;

    timer.start([&count] { count++; }, std::chrono::milliseconds(10));

    loop.run();

    EXPECT_EQ(1, count);
}

// repeat use case
TEST(TimerTest, Repeat) {
    int count = 0;
    uvx::timer timer;
    uvx::loop loop;

    auto time = std::chrono::milliseconds(10);
    timer.start([&count, &loop] {
        if (++count >= 5)
            loop.stop();
    }, time, time);

    loop.run();

    EXPECT_EQ(5, count);
}

// stop timer
TEST(TimerTest, Stop) {
    int count = 0;
    uvx::timer timer;
    uvx::loop loop;

    timer.start([&count] { count++; }, std::chrono::milliseconds(10));
    timer.stop();

    loop.run();

    EXPECT_EQ(0, count);
}

// again timer
TEST(TimerTest, Again) {
    int count = 0;
    uvx::timer timer;
    uvx::loop loop;

    auto result = timer.again();
    EXPECT_NE(0, result);

    auto time = std::chrono::milliseconds(10);
    timer.start([&count, &loop] {
        if (++count >= 5) {
            loop.stop();
        }
    }, time, time);

    loop.run();

    count = 0;
    timer.again();

    loop.run();

    EXPECT_EQ(5, count);
}

// called after closed
TEST(TimerTest, CallAfterClosed) {
    int count = 0;
    uvx::timer timer;
    uvx::loop loop;

    timer.close();
    EXPECT_THROW(timer.start([&count] { count++; }, std::chrono::milliseconds(10)),
                 std::runtime_error) << "not throw exception runtime_error";
    EXPECT_THROW(timer.stop(), std::runtime_error) << "not throw exception runtime_error";

    loop.run();

    EXPECT_EQ(0, count);
}

}  // namespace
