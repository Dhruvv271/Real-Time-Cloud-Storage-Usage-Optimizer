#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
    std::queue<T> q;
    std::mutex m;
    std::condition_variable cv;

public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(m);
        q.push(item);
        cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&]{ return !q.empty(); });
        T item = q.front();
        q.pop();
        return item;
    }
};
