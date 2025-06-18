#pragma once 

#include <utility>
#include <type_traits>

template <class T>
struct DefaultDeleter {
    void operator()(T* p) const {
        delete p;
    }
};

template <class T>
struct DefaultDeleter<T[]> {
    void operator()(T* p) const {
        delete[] p;
    }
};

template <class T, class Deleter = DefaultDeleter<T>>
class UniquePtr {
    template <class U, class UDeleter>
    friend class UniquePtr;

public:
    UniquePtr(std::nullptr_t = nullptr) noexcept : _p(nullptr) {}

    explicit UniquePtr(T* p) noexcept : _p(p) {}

    template <class U, class UDeleter> requires(std::is_convertible_v<U*, T*>)
    UniquePtr(UniquePtr<U, UDeleter>&& that) noexcept 
        : _p(that._p) {
        that._p = nullptr;
    }

    ~UniquePtr() noexcept {
        if (_p)
            _deleter(_p);
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& that) noexcept : _p(that._p) {
        that._p = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& that) noexcept {
        if (this != &that) {
            if (_p)
                _deleter(_p);
            _p = that._p;
            that._p = nullptr;
        }
        return *this; 
    }

    void swap(UniquePtr& that) noexcept { std::swap(_p, that._p); }

    T* get() const noexcept { return _p; }

    T* operator->() const noexcept { return _p; }

    std::add_lvalue_reference_t<T> operator*() const noexcept { return *_p; }

    Deleter getDeleter() const noexcept { return _deleter; }

    T* release() noexcept {
        T* p = _p;
        _p = nullptr;
        return p;
    }

    void reset(T* p = nullptr) noexcept {
        if (_p)
            _deleter(_p);
        _p = p;
    }

    explicit operator bool() const noexcept { return _p != nullptr; }

    bool operator==(const UniquePtr& that) const { return _p == that._p; }

    bool operator!=(const UniquePtr& that) const { return !(*this == that); }

    bool operator<(const UniquePtr& that) const { return _p < that._p; }

    bool operator<=(const UniquePtr& that) const { return _p <= that._p; }

    bool operator>(const UniquePtr& that) const { return _p > that._p; }

    bool operator>=(const UniquePtr& that) const { return _p >= that._p; }

private:
    T* _p;
    Deleter _deleter;
};

template <class T, class Deleter>
class UniquePtr<T[], Deleter> : public UniquePtr<T, Deleter> {
public:
    using UniquePtr<T, Deleter>::UniquePtr;

    std::add_lvalue_reference_t<T> operator[](std::size_t i) {
        return this->get()[i];
    }
};

template <class T, class ...Args> requires(!std::is_unbounded_array_v<T>)
UniquePtr<T> makeUnique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
