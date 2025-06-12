#pragma once 

#include <atomic>
#include <memory>

template <class T>
class SharedPtr {
public:
    SharedPtr(const SharedPtr& that) noexcept 
        : _p(that._p), _refs(that._refs) {
        incref();
    }

    SharedPtr& operator=(const SharedPtr& that) noexcept {
        if (this == &that)
            return *this;
        decref();
        _p = that._p;
        _refs = that._refs;
        incref();
        return *this;
    }

    SharedPtr(const SharedPtr&& that) noexcept 
        : _p(that._p), _refs(that._refs) {
        that._p = nullptr;
        that._refs = nullptr;
    }

    SharedPtr& operator=(const SharedPtr&& that) noexcept {
        if (this == &that)
            return nullptr;
        decref();
        _p = that._p;
        _refs = that._refs;
        that._p = nullptr;
        that._refs = nullptr;
        return *this;
    }

private:
    void incref() {
        if (_refs)
            _refs->fetch_add(1, std::memory_order_relaxed);
    }

    void decref() {
        if (_refs && _refs->fetch_sub(1, std::memory_order_relaxed) == 1)
            delete _p; 
    }

private:
    T* _p;
    std::atomic<size_t>* _refs;
};
