#include <gtest/gtest.h>

#include "uvx/work.hpp"
#include "uvx/loop.hpp"

namespace {

// normal case
TEST(WorkTest, NormalCase) {
    int count = 0;
    uvx::work work;
    uvx::loop loop;

    work.execute([&count] { count++; }, [&count](int) { count++; });

    loop.run();

    EXPECT_EQ(2, count);
}

// cancel
TEST(WorkTest, Cancel) {
    int count = 0;
    uvx::work work;
    uvx::loop loop;

    // callback will be called with status UV_ECANCELED
    work.execute([&count] { count++; }, [&count](int status) { EXPECT_EQ(UV_ECANCELED, status); count += 5; });
    work.cancel();

    loop.run();

    EXPECT_EQ(5, count);
}

// call after move
TEST(WorkTest, CallAfterMove) {
    int count = 0;
    uvx::work work;
    uvx::work tmp = std::move(work);
    uvx::loop loop;

    EXPECT_THROW(work.execute([&count] { count++; }, [&count](int) { count++; }),
                 std::runtime_error) << "not throw exception std::runtime_error";

    loop.run();

    EXPECT_EQ(0, count);
}


}  // namespace
