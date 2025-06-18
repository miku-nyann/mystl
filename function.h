#pragma once 

#include "unique_ptr.h"

#include <utility> // forward  in_place
#include <type_traits> // decay_t 
#include <functional> // invoke

template <class Ret, class ...Args>
class Function 
{
    struct FuncBase {
        virtual Ret call(Args... args) = 0;
        virtual UniquePtr<FuncBase> clone() const = 0;
        virtual ~FuncBase() = default;
    };

    template <class Fn>
    struct FuncImpl : FuncBase {
        template <class ...CArgs>
        explicit FuncImpl(std::in_place_t, CArgs&& ... args) : _f(std::forward<CArgs>(args)...) {}

        Ret call(Args ...args) override {
            return std::invoke(_f, std::forward<Args>(args)...);
        }

        UniquePtr<FuncBase> clone() const override {
            return makeUnique<FuncImpl>(std::in_place, _f);
        }

        Fn _f;
    };

public:
    Function() = default;
    Function(std::nullptr_t) noexcept : Function() {}

    template <class Fn>
    requires(std::is_invocable_r_v<Ret, std::decay_t<Fn>, Args...> &&
        std::is_copy_constructible_v<Fn> &&
        !std::is_same_v<std::decay_t<Fn>, Function<Ret(Args...)>>)
    Function(Fn&& f) 
        : _base(makeUnique<FuncImpl<std::decay_t<Fn>>(std::in_place, std::forward<Fn>(f)))
    {}

    Function(Function&&) = default;
    Function& operator=(Function&&) = default;

    Function(const Function& that)
        : _base(that._base ? that._base->clone() : nullptr) {}

    Function& operator=(const Function& that) {
        if (that._base)
            _base = that._base->clone();
        else 
            _base = nullptr;
    }

    explicit operator bool() const noexcept {
        return _base != nullptr;
    }

    bool operator==(std::nullptr_t) const noexcept {
        return _base == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept {
        return _base != nullptr;
    }

    Ret operator()(Args... args) const {
        if (!_base)
            throw std::bad_function_call();
        return _base->call(std::forward(args)...);
    }

private:
    UniquePtr<Function> _base;
};
