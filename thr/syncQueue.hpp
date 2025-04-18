#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <unistd.h>

#ifndef SYNCQUEUE_HPP
#define SYNCQUEUE_HPP

template <class Task>
class SyncQueue
{
    std::queue<Task> que_;
    mutable std::mutex mtx_;
    std::condition_variable cv_que_empty_;
    std::condition_variable cv_que_full_;
    std::condition_variable cv_que_not_empty_; // 拿完再走，优雅停止
    size_t max_que_size_;
    std::chrono::milliseconds interval_wait_for_put_;
    std::chrono::seconds interval_wait_for_take_;
    std::chrono::milliseconds interval_wait_for_stop_;
    bool is_que_running_;

private:
    bool isFull() const { return que_.size() >= max_que_size_; }
    bool isEmpty() const { return que_.empty(); }
    // rt:
    //   0   sucess
    //   -1  timeout
    //   -2  task queue stopped
    template <class Func>
    int push_r(Func &&task)
    {
        std::unique_lock<std::mutex> locker(mtx_);
        while (is_que_running_ && isFull())
        {
            if (std::cv_status::timeout ==
                cv_que_full_.wait_for(locker, interval_wait_for_put_))
                return -1; // 满了不被叫等10毫秒就返回
        }
        if (!is_que_running_)
            return -2;
        que_.push(std::forward<Func>(task));
        cv_que_empty_.notify_all();
        return 0;
    }

public:
    SyncQueue(size_t max_que_size)
        : max_que_size_(max_que_size),
          interval_wait_for_put_(10),
          interval_wait_for_take_(1),
          interval_wait_for_stop_(1),
          is_que_running_(true) {}
    ~SyncQueue() {}
    void stop_r()
    {
        is_que_running_ = false; // 只会被改成假，不加锁
        std::unique_lock<std::mutex> locker(mtx_);
        while (!isEmpty()) // 每1毫秒查看一下拿空没，没有就弃锁继续睡
        {
            cv_que_empty_.notify_all();
            cv_que_full_.notify_all();
            cv_que_not_empty_.wait_for(locker, interval_wait_for_stop_);
        }
    }
    // rt:
    //   0   sucess
    //   -1  timeout
    //   -2  task queue stopped
    int put_r(Task &&task)
    {
        return push_r(std::forward<Task>(task)); // 确保值类别被正确传递
    }
    // rt:
    //   0   sucess
    //   -1  timeout
    //   -2  task queue stopped
    int put_r(const Task &task)
    {
        return push_r(task);
    }
    // rt:
    //   true   sucess
    //   false  timeout
    bool take_r(Task &task) // take_r函数必须通过引用传递方式取Task，防止返回值传递方式超越locker生命周期引起的数据竞争（move或RVO）
    {
        std::unique_lock<std::mutex> locker(mtx_);
        while (isEmpty())
        {
            if (std::cv_status::timeout ==
                cv_que_empty_.wait_for(locker, interval_wait_for_take_))
                return false;
        }
        task = que_.front();
        que_.pop();
        cv_que_full_.notify_all();
        return true;
    }
    // rt:
    //   true   sucess
    //   false  timeout
    bool take_r(std::deque<Task> &task_que)
    {
        std::unique_lock<std::mutex> locker(mtx_);
        while (isEmpty())
        {
            if (std::cv_status::timeout ==
                cv_que_empty_.wait_for(locker, interval_wait_for_take_))
                return false;
        }
        task_que = std::move(que_);
        cv_que_full_.notify_all();
        return true;
    }
    bool empty_r() const
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return que_.empty();
    }
    bool full_r() const
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return que_.size() >= max_que_size_;
    }
    size_t size_r() const
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return que_.size();
    }
};

#endif
