#pragma once
#ifndef BUFFERED_CHANNEL_H
#define BUFFERED_CHANNEL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <optional>

template<typename T>
class BufferedChannel {
private:
    std::queue<T> buffer_;
    size_t capacity_;
    bool closed_ = false;
    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;

public:
    explicit BufferedChannel(size_t capacity) : capacity_(capacity) {}

    bool send(T value) {
        std::unique_lock<std::mutex> lock(mutex_);

        not_full_.wait(lock, [this]() {
            return buffer_.size() < capacity_ || closed_;
            });

        if (closed_) {
            throw std::runtime_error("Cannot send to closed channel");
        }

        buffer_.push(std::move(value));
        not_empty_.notify_one();
        return true;
    }

    std::pair<T, bool> recv() {
        std::unique_lock<std::mutex> lock(mutex_);

        not_empty_.wait(lock, [this]() {
            return !buffer_.empty() || closed_;
            });

        if (!buffer_.empty()) {
            T value = std::move(buffer_.front());
            buffer_.pop();
            not_full_.notify_one();
            return { std::move(value), true };
        }

        return { T(), false };
    }

    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
        not_empty_.notify_all();
        not_full_.notify_all();
    }

    bool is_closed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_ && buffer_.empty();
    }
};

#endif
