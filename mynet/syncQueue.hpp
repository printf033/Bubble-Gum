#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <unistd.h>

#ifndef SYNCQUEUE_HPP
#define SYNCQUEUE_HPP

// 同步队列最大容量
#define MAX_QUE_SIZE 10000
// 同步队列put最大等待时间（ms）
#define MAX_PUT_WAIT_TIME 10
// 同步队列take最大等待时间（ms）
#define MAX_TAKE_WAIT_TIME 1000

template <class Element>
class SyncQueue
{
    std::queue<Element> que_;
    mutable std::mutex mtx_;
    std::condition_variable cv_que_empty_;
    std::condition_variable cv_que_full_;
    bool is_que_running_;

public:
    SyncQueue() : is_que_running_(true) {}
    ~SyncQueue() { stop_r(); }
    SyncQueue(const SyncQueue &) = delete;
    SyncQueue &operator=(const SyncQueue &) = delete;
    SyncQueue(SyncQueue &&) = delete;
    SyncQueue &operator=(SyncQueue &&) = delete;
    void stop_r()
    {
        is_que_running_ = false; // 只会被改成假，不加锁
        cv_que_full_.notify_all();
        for (int i = 0; i < 3 && !isEmpty(); ++i) // 3次循环等待
        {
            cv_que_empty_.notify_all();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::clog << "sync queue: remaining elements size " << size_r() << std::endl;
    }
    // rt:
    //   0   sucess
    //   -1  timeout
    //   -2  element queue stopped
    template <class T>
    int put_r(T &&element)
    {
        std::unique_lock<std::mutex> locker(mtx_);
        while (is_que_running_ && isFull())
        {
            if (std::cv_status::timeout ==
                cv_que_full_.wait_for(locker, std::chrono::milliseconds(MAX_PUT_WAIT_TIME)))
                return -1;
        }
        if (!is_que_running_)
            return -2;
        que_.push(std::forward<T>(element));
        cv_que_empty_.notify_all();
        return 0;
    }
    // rt:
    //   0   sucess
    //   -1  timeout
    int take_r(Element &element) // take_r函数必须通过引用传递方式取Element，防止返回值传递方式超越locker生命周期引起的数据竞争（move或RVO）
    {
        std::unique_lock<std::mutex> locker(mtx_);
        while (isEmpty())
        {
            if (std::cv_status::timeout ==
                cv_que_empty_.wait_for(locker, std::chrono::milliseconds(MAX_TAKE_WAIT_TIME)))
                return -1;
        }
        element = que_.front();
        que_.pop();
        cv_que_full_.notify_all();
        return 0;
    }
    // rt:
    //   0   sucess
    //   -1  timeout
    int take_r(std::deque<Element> &que)
    {
        std::unique_lock<std::mutex> locker(mtx_);
        while (isEmpty())
        {
            if (std::cv_status::timeout ==
                cv_que_empty_.wait_for(locker, std::chrono::milliseconds(MAX_TAKE_WAIT_TIME)))
                return -1;
        }
        que = std::move(que_);
        cv_que_full_.notify_all();
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
