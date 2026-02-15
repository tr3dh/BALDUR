#pragma once

#include "__Asserts.h"

#include <mutex>
#include <utility>

template<typename T>
class ThreadSafe {
private:
    mutable std::mutex mutex_;
    T data_;
    
public:
    class Proxy {
        std::unique_lock<std::mutex> lock_;
        T* data_;
    public:
        Proxy(std::mutex& m, T* d) : lock_(m), data_(d) {}
        T* operator->() { return data_; }
        T& operator*() { return *data_; }
    };
    
    ThreadSafe() = default;
    ThreadSafe(const T& val) : data_(val) {}
    ThreadSafe(T&& val) : data_(std::move(val)) {}
    
    Proxy operator->() { return Proxy(mutex_, &data_); }
    Proxy lock() { return Proxy(mutex_, &data_); }
};