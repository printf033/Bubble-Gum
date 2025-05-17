#include <queue>
#include <mutex>
#include <chrono>
#include <iostream>
#include <unistd.h>

#ifndef SYNCQUEUE_NONBLOCKING_HPP
#define SYNCQUEUE_NONBLOCKING_HPP

// 同步队列最大容量
#define MAX_QUE_SIZE 10000

template <class Element>
class SyncQueue_nonblocking
{
    std::queue<Element> que_;
    mutable std::mutex mtx_;
    bool is_que_running_; // 客户线程会持续拿，不需要原子变量

public:
    SyncQueue_nonblocking() : is_que_running_(true) {}
    ~SyncQueue_nonblocking()
    {
        is_que_running_ = false;
        while (!empty_r())
            std::clog << "sync queue: remaining elements size " << size_r() << std::endl;
    }
    SyncQueue_nonblocking(const SyncQueue_nonblocking &) = delete;
    SyncQueue_nonblocking &operator=(const SyncQueue_nonblocking &) = delete;
    SyncQueue_nonblocking(SyncQueue_nonblocking &&) = delete;
    SyncQueue_nonblocking &operator=(SyncQueue_nonblocking &&) = delete;
    // rt:
    //   0   sucess
    //   -1  queue is full
    //   -2  queue stopped
    template <class T>
    int put_r(T &&element)
    {
        std::unique_lock<std::mutex> locker(mtx_);
        if (!is_que_running_)
            return -2;
        if (isFull())
            return -1;
        que_.push(std::forward<T>(element));
        return 0;
    }
    // rt:
    //   0   sucess
    //   -1  queue is empty
    int take_r(Element &element) // take_r函数必须通过引用传递方式取Element，防止返回值传递方式超越locker生命周期引起的数据竞争（move或RVO）
    {
        std::unique_lock<std::mutex> locker(mtx_);
        if (isEmpty())
            return -1;
        element = que_.front();
        que_.pop();
        return 0;
    }
    // rt:
    //   0   sucess
    //   -1  queue is empty
    int take_r(std::deque<Element> &que)
    {
        std::unique_lock<std::mutex> locker(mtx_);
        if (isEmpty())
            return -1;
        que = std::move(que_);
        return 0;
    }
    bool empty_r() const
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return que_.empty();
    }
    bool full_r() const
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return que_.size() >= MAX_QUE_SIZE;
    }
    size_t size_r() const
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return que_.size();
    }

private:
    bool isFull() const { return que_.size() >= MAX_QUE_SIZE; }
    bool isEmpty() const { return que_.empty(); }
};

#endif
