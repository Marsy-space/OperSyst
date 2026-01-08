#pragma once
#ifndef BUFFERED_CHANNEL_H
#define BUFFERED_CHANNEL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <utility>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(size_t buffer_size)
        : buffer_size_(buffer_size), closed_(false) {
    }

    void send(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);

        if (closed_) {
            throw std::runtime_error("Channel is closed");
        }

        send_cv_.wait(lock, [this]() {
            return closed_ || queue_.size() < buffer_size_;
            });

        if (closed_) {
            throw std::runtime_error("Channel is closed");
        }

        queue_.push(value);

        recv_cv_.notify_one();
    }

    std::pair<T, bool> recv() {
        std::unique_lock<std::mutex> lock(mutex_);

        recv_cv_.wait(lock, [this]() {
            return closed_ || !queue_.empty();
            });

        if (!queue_.empty()) {
            T value = std::move(queue_.front());
            queue_.pop();

            send_cv_.notify_one();

            return { std::move(value), true };
        }

        return { T(), false };
    }

    void close() {
        std::unique_lock<std::mutex> lock(mutex_);
        closed_ = true;

        send_cv_.notify_all();
        recv_cv_.notify_all();
    }

    ~BufferedChannel() {
        close();
    }

private:
    std::queue<T> queue_;
    size_t buffer_size_;
    bool closed_;

    std::mutex mutex_;
    std::condition_variable send_cv_;
    std::condition_variable recv_cv_;
};

#endif // BUFFERED_CHANNEL_H