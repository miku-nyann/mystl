#pragma once 

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
public:
    explict UniquePtr(T* p) noexcept : _p(p) {}

    ~UniquePtr() noexcept {
        if (_p)
            _deleter(_p);
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(Unique&& that) noexcept : _p(that._p) {
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

    T* get() const noexcept { return _p; }

    T* operator->() const noexcept() { return _p; }

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

    explicit operator bool() const noexcept {
        return _p != nullptr;
    }

    bool operator==(const UniquePtr& that) const {
        return _p == that._p;
    }

    bool operator!=(const UniquePtr& that) const {
        return !(*this == that);
    }

    bool operator<(const UniquePtr& that) const {
        return _p < that._p;
    }

    bool operator<=(const UniquePtr& that) const {
        return _p <= that._p;
    }

    bool operator>(const UniquePtr& that) const {
        return _p > that._p;
    }

    bool operator>=(const UniquePtr& that) const {
        return _p >= that._p;
    }

private:
    T* _p;
    Deleter _deleter;
};
