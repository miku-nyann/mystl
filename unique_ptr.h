#pragma once 

template <class T>
class DefaultDeleter {
    void operator()(T* p) const {
        delete p;
    }
};

template <class T, class Deleter = DefaultDeleter<T>>
class UniquePtr 
{
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

        }
        return *this;
    }

    T* get() const noexcept { return _p; }

    T* operator->() const noexcept() { return _p; }

    

private:
    T* _p;
    Deleter _deleter;
};
