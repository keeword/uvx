#pragma once

#include <any>
#include <unordered_map>
#include <type_traits>

#include "functype.hpp"

namespace utils {

class methods {
private:
    using method_type = std::function<std::any(std::any)>;

    template <typename R, typename F>
    struct invoker {
        static R invoke(const F& f, std::any any) {
            using argument_tuple = typename functype::traits<F>::argument_tuple;
            return std::apply(f, std::any_cast<argument_tuple>(std::move(any)));
        }
    };

    template <typename F>
    struct invoker<void, F> {
        static std::any invoke(const F& f, std::any any) {
            using argument_tuple = typename functype::traits<F>::argument_tuple;
            std::apply(f, std::any_cast<argument_tuple>(std::move(any)));
            return std::any();
        }
    };

public:
    template <typename Func>
    void store(std::size_t id, Func func) {
        static_assert(std::is_copy_constructible<Func>::value, "method must be CopyConstructible");
        static_assert(std::is_move_constructible<Func>::value, "method must be MoveConstructible");

        if (map_.count(id)) { // delete the already exist one
            map_.erase(id);
        }
        map_.emplace(id, [func = std::move(func)](std::any any)->std::any {
            using result_type = typename functype::traits<Func>::result_type;
            return invoker<result_type, Func>::invoke(func, std::move(any)); }
        );
    }

    // exception:
    // bad_any_cast: argument type not match the method
    // out_of_range: method not exist
    template <typename... Args>
    decltype(auto) invoke(std::size_t id, Args&&... args) {
        return map_.at(id)(std::tuple<typename std::remove_reference<Args>::type...>(std::forward<Args>(args)...));
    }

    bool exist(std::size_t id) {
        return !!map_.count(id);
    }

private:
    std::unordered_map<std::size_t, method_type> map_;
};

} // namespace utils
