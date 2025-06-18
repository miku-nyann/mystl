#pragma once 

#include <atomic>
#include <memory>
#include <type_traits>

#include "unique_ptr.h"

struct SpCounter {
    SpCounter() noexcept : _refs(1) {}

    virtual ~SpCounter() = default;

    SpCounter(SpCounter&&) = delete;

    void incref() noexcept {
        _refs.fetch_add(1, std::memory_order_relaxed);
    }

    void decref() noexcept {
        _refs.fetch_sub(1, std::memory_order_relaxed);
    }

    long refs() const noexcept {
        return _refs.load(std::memory_order_relaxed);
    }

    std::atomic<long> _refs;
};

template <class T, class Deleter>
struct SpCounterImpl final : SpCounter {
    explicit SpCounterImpl(T* ptr) noexcept : _p(p) {}

    explicit SpCounterImpl(T* ptr, Deleter deleter) noexcept 
        : _p(ptr), _deleter(deleter) {}

    ~SpCounterImpl() noexcept override {
        _deleter(_p);
    }

    T* _p;
    Deleter _deleter;
};

template <class T>
class SharedPtr {
    template <class>
    friend class SharedPtr;

public:
    SharedPtr(std::nullptr_t = nullptr) noexcept : _owner(nullptr) {}

    template <class U> requires(std::is_convertible_v<U*, T*>)
    explicit SharedPtr(U* p) 
        : _p(p), _owner(new SpCounterImpl<U, DefaultDeleter<U>(_p)) {
        // TODO
    }

    template <class U, class UDeleter> requires(std::is_convertible_v<U*, T*>)
    explicit SharedPtr(U* p, UDeleter deleter)
        : _p(p), _owner(new SpCounterImpl<U, UDeleter>(_p, std::move(deleter))) {
        // TODO
    }

    template <class U, class UDeleter> requires(std::is_convertible_v<U*, T*>)
    explicit SharedPtr(SharedPtr<U, UDeleter>&& that)
        : SharedPtr(that.release(), that.getDeleter()) {}

    SharedPtr(const SharedPtr& that) noexcept
        : _p(that._p), _owner(that._owner) {
        if (_owner)
            _owner->incref();
    }

    template <class U> requires(std::is_convertible_v<U*, T*>)
    SharedPtr(SharedPtr<U>& that) noexcept 
        : _p(that._p), _owner(that._owner) {
        if (_owner)
            _owner->incref();
    }

    SharedPtr(SharedPtr&& that) noexcept 
        : _p(that._p), _owner(that._owner) {
        that._p = that._owner = nullptr;
    }

    template <class U> requires(std::is_convertible_v<U*, T*>)
    SharedPtr(SharedPtr<U>&& that) noexcept 
        : _p(that._p), _owner(that._owner) {
        that._p = nullptr;
        that._owner = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& that) noexcept {
        if (this == &that)
            return *this;
        if (_owner)
            _owner->decref();
        _p = that._p;
        _owner = that._owner;
        if (_owner)
            _owner->incref();
        return *this;
    }

    template <class U> requires(std::is_convertible_v<U*, T*>)
    SharedPtr& operator=(const SharedPtr<U>& that) noexcept {
        if (this == &that)
            return *this;
        if (_owner)
            _owner->decref();
        _p = that._p;
        _owner = that._owner;
        if (_owner)
            _owner->incref();
        return *this;
    }

    template <class U> requires(std::is_convertible_v<U*, T*>)
    SharedPtr& operator=(const SharedPtr<U>&& that) noexcept {
        if (this == &that)
            return *this;
        if (_owner)
            _owner->decref();
        _p = that._p;
        _owner = that._owner;
        that._p = that._owner = nullptr;
    }

    void reset() noexcept {
        if (_owner)
            _owner->decref();
        _p = _owner = nullptr;
    }

    template <class U>
    void reset<U* p) noexcept {
        if (_owner)
            _owner->decref();
        _p = p;
        _owner = new SpCounterImpl<U, DefaultDeleter<U>>(_p);
        // TODO
    }

    template <class U, class UDeleter>
    void reset(U* p, Deleter deleter) {
        if (_owner)
            _owner->decref();
        _p = p;
        _owner = new SpCounterImpl<U, UDeleter>(p, std::move(deleter));
        // TODO
    }

    ~SharedPtr() noexcept {
        if (_owner)
            _owner->decref();
    }

    long useCount() noexcept {
        return _owner ? _owner->refs() : 0;
    }

    bool unique() noexcept {
        return _owner ? _owner->refs() == 1 : true;
    }

    template <class U>
    bool operator==(const SharedPtr<U>& that) const noexcept {
        return _p == that._p;
    }

    template <class U>
    bool operator!=(const SharedPtr<U>& that) const noexcept {
        return !(*this == that);
    }

    template <class U>
    bool operator<(const SharedPtr<U>& that) const noexcept {
        return _p < that._p;
    }

    template <class U>
    bool operator<=(const SharedPtr<U>& that) const noexcept {
        return _p <= that._p;
    }

    template <class U>
    bool operator>(const SharedPtr<U>& that) const noexcept {
        return _p > that._p;
    }

    template <class U>
    bool operator>=(const SharedPtr<U>& that) const noexcept {
        return _p >= that._p;
    }

    template <class U>
    bool ownerBefore(const SharedPtr<U>& that) const noexcept {
        return _owner < that._owner;
    }

    template <class U>
    bool ownerEqual(const SharedPtr<U>& that) const noexcept {
        return _owner == that._owner;
    }

    void swap(SharedPtr& that) noexcept {
        swap(_p, that._p);
        swap(_owner, that._owner);
    }

    T* get() const noexcept { return _p; }

    T* operator->() const noexcept { return _p; }

    std::add_lvalue_reference_t<T> operator*() const noexcept { return *_p; }

    explicit operator bool() const noexcept { return _p != nullptr; }

private:
    SharedPtr(T* p, SpCounter* owner) noexcept : _p(p), _owner(owner) {}

private:
    T* _p;
    SpCounter* _owner;
};
