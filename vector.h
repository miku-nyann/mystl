#pragma once 

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

template <class T>
class Vector {
public:
    Vector() : _data(nullptr), _size(0), _capacity(0) {}

    Vector(std::initializer_list<T>& ilist) : Vector(ilist.begin(), ilist.end()) {}
    
    template <class InputIter>
    Vector(InputIter& first, InputIter& last) {
        size_t n = last - first;
        _data = (T*)malloc(n * sizeof(T));
        _capacity = _size = n;
        for (size_t i = 0; i != _size; ++i) {
            _data[i] = *first;
            ++first;
        }
    }

    ~Vector() { free(_data); }

    Vector(const Vector& that) {
        size_t n = that._size;
        _data = (T*)malloc(n * sizeof(T));
        _capacity = _size = n;
        for (size_t i = 0; i != _size; ++i) {
            _data[i] = that._data[i];
        }
    }

    Vector& operator=(const Vector& that) {
        if (this == &that)
            return *this;
        reserve(that._size);
        _size = that._size;
        for (int i = 0; i != _size; ++i)
            _data[i] = that._data[i];
    }

    Vector(Vector&& that) {
        _data = that._data;
        _size = that._size;
        _capacity = that._capacity;
        that._data = nullptr;
        that._size = 0;
        that._capacity = 0;
    }

    Vector& operator=(Vector&& that) {
        if (this == &that)
            return *this;
        free(_data);
        _data = that._data;
        _size = that._size;
        _capacity = that._capacity;
        that._data = nullptr;
        that._size = 0;
        that._capacity = 0;
    }

    const T* data() const { return _data; }
    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }

    void resize(size_t n, const T& val) {
        if (_size < n) {
            reserve(n);
            for (size_t i = _size; i != n; ++i)
                _data[i] = val;
        }
        _size = n;
    }

    void reserve(size_t n) {
        if (_capacity >= n)
            return;
        _capacity = std::max(n, _capacity * 2);
        T* oldData = _data;
        _data = (T*)malloc(_capacity * sizeof(T));
        for (size_t i = 0; i != _size; ++i)
            _data[i] = oldData[i];
        free(oldData);
    }

    void clear() { _size = 0; }
    void empty() const { _size == 0; }

    T& front() { return *_data; }
    const T& front() const { return *_data; }

    T& back() { return _data[_size - 1]; }
    const T& back() const { return _data[_size - 1]; }

    T& operator[](size_t i) { return _data[i]; }
    const T& operator[](size_t i) const { return _data[i]; }

    T& at(size_t i) {
        if (i >= _size) [[unlikely]]
            throw std::out_of_range("Vector::at");
        return _data[i];
    }

    const T& at(size_t i) const {
        if (i >= _size) [[unlikely]]
            throw std::out_of_range("Vector::at");
        return _data[i];
    }

    void pushBack(const T& val) {
        if (_size + 1 >= _capacity) [[unlikely]]
            reserve(_size + 1);
        _data[_size] = val;
        ++_size;
    }

    void pushBack(T&& val) {
        if (_size + 1 >= _capacity) [[unlikely]]
            reserve(_size + 1);
        _data[_size] = std::move(val);
        ++_size;
    }

    void popBack() { --_size; }

    T* erase(const T* it) {
        size_t i = it - _data;
        for (size_t j = i + 1; j != _size; ++j) {
            _data[j - 1] = std::move(_data[j]);
        }
        --_size;
        return const_cast<T*>(it);
    }

    T* erase(const T* first, const T* last) {
        size_t i = first - _data;
        size_t delta = last - first;
        for (size_t j = i + 1; j != _size; ++j) {
            _data[j - delta] = std::move(_data[j]);
        }
        _size -= delta;
        return const_cast<T*>(first);
    }

    T* insert(const T* it, T&& val) {
        size_t i = it - _data;
        if (_size + 1 > _capacity) [[unlikely]] 
            reserve(_size + 1);
        for (size_t j = _size; j != i; --j) 
            _data[j] = _data[j - 1];
        _data[i] = std::move(val);
        ++_size;
        return _data + i;
    }

    T* insert(const T* it, const T& val) {
        size_t i = it - _data;
        if (_size + 1 > _capacity)
            reserve(_size + 1);
        for (size_t j = _size; j != i; --j)
            _data[j] = _data[j - 1];
        _data[i] = val;
        ++_size;
        return _data + i;
    }

    T* insert(const T* it, size_t n, const T& val) {
        size_t i = it - _data;
        if (n == 0)
            return const_cast<T*>(it);
        reserve(_size + n);
        for (size_t j = _size + n - 1; j != i + n - 1; --j)
            _data[j] = _data[j - n];
        for (size_t j = i + n; j != i; --j)
            _data[j] = val;
        _size += n;
        return _data + i;
    }

private:
    T* _data;
    size_t _size;
    size_t _capacity;
};
