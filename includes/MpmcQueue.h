#ifndef MPMCQUEUE_H
#define MPMCQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

// Bounded Multi-Producer Multi-Consumer queue.
// Producers block when full; consumers block when empty.
template<typename T>
class MpmcQueue {
public:
    explicit MpmcQueue(std::size_t capacity) : capacity_(capacity) {}

    // Block until space is available, then enqueue.
    void push(T item)
    {
        std::unique_lock lock(mutex_);

        // Wait until the queue has space OR shutdown is requested
        cv_not_full_.wait(lock, [this]
                          { return queue_.size() < capacity_ || stopped_; });
        if (stopped_)
        {
            return;
        }
        queue_.push(std::move(item));
        cv_not_empty_.notify_one();
    }

    // Block until an item is available. Returns nullopt if stopped & empty.
    std::optional<T> pop()
    {
        std::unique_lock lock(mutex_);
        cv_not_empty_.wait(lock, [this]
                           { return !queue_.empty() || stopped_; });

        if (queue_.empty())
        {
            return std::nullopt;
        }

        T item = std::move(queue_.front());
        queue_.pop();
        cv_not_full_.notify_one();
        return item;
    }

    // Non-blocking try-pop. Returns nullopt immediately if empty.
    std::optional<T> try_pop()
    {
        std::unique_lock lock(mutex_);

        if (queue_.empty())
        {
            return std::nullopt;
        }

        T item = std::move(queue_.front());
        queue_.pop();
        cv_not_full_.notify_one();
        return item;
    }

    void stop()
    {
        std::unique_lock lock(mutex_);
        stopped_ = true;
        cv_not_empty_.notify_all();
        cv_not_full_.notify_all();
    }

    std::size_t size() const
    {
        std::unique_lock lock(mutex_);
        return queue_.size();
    }

    bool empty() const
    {
        std::unique_lock lock(mutex_);
        return queue_.empty();
    }

private:
    std::size_t             capacity_;
    std::queue<T>           queue_;
    mutable std::mutex      mutex_;
    std::condition_variable cv_not_empty_;
    std::condition_variable cv_not_full_;
    bool                    stopped_ = false;
};

#endif // MPMCQUEUE_H