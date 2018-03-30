#include "uvx/method.hpp"
#include <gtest/gtest.h>

namespace {

struct CanNotCopy {
	CanNotCopy() {}
	~CanNotCopy() {}

	CanNotCopy(const CanNotCopy&) { ADD_FAILURE() << "no copy constructor"; }
	CanNotCopy& operator=(const CanNotCopy&) { ADD_FAILURE() << "no copy assigment"; }

	CanNotCopy(CanNotCopy&&) {}
	CanNotCopy& operator=(CanNotCopy&&) {}
};

// normal use case
TEST(MethodTest, NormalCase) {
    utils::methods methods;
    methods.store(0, [](int a, int b) { return a + b; });
    int sum = std::any_cast<int>(methods.invoke(0, 10, 20));
    EXPECT_TRUE(30 == sum);
}

// overwrite the method
TEST(MethodTest, OverWrite) {
    utils::methods methods;
    methods.store(0, [](int a, int b) { return a + b; });
    methods.store(0, [](int a, int b) { return a * b; });
    int sum = std::any_cast<int>(methods.invoke(0, 10, 20));
    EXPECT_TRUE(200 == sum);
}

// no copy inside the implementation
TEST(MethodTest, MoveOnly) {
    utils::methods methods;
    CanNotCopy cannotcopy;
    methods.store(10, [obj = std::move(cannotcopy)](CanNotCopy) {});
    methods.invoke(10, CanNotCopy());
}

// try to invoke method not exist
TEST(MethodTest, InvokeNotExist) {
    utils::methods methods;
    EXPECT_THROW(methods.invoke(20), std::out_of_range) << "not throw exception out_of_range";
}

// try to invoke method with argument type not match
TEST(MethodTest, BadArgumentType) {
    utils::methods methods;
    methods.store(30, [](int) {});
    EXPECT_THROW(methods.invoke(30, 30.0), std::bad_any_cast) << "not throw exception bad_any_cast";
}

}  // namespace
