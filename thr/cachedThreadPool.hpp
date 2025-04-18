#include "syncQueue.hpp"
#include <functional>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iostream>
#include <future>

#ifndef CACHEDTHREADPOOL_HPP
#define CACHEDTHREADPOOL_HPP

// 缓存线程池最大任务队列大小
#define MAX_TASKQUEUE_SIZE 10000
// 缓存线程池最大线程数
#define MAX_THREAD_SIZE 1000
// 缓存线程池最大线程数
#define MIN_THREAD_SIZE 2
// 缓存线程池空闲线程最大存活时间（单位：s）
#define MAX_THREAD_IDLE_TIME 5

class CachedThreadPool
{
public:
    using Task = std::function<void(void)>;

private:
    SyncQueue<Task> task_que_;
    std::unordered_map<std::thread::id, std::unique_ptr<std::thread>> map_tid_puThr_;
    std::mutex mtx_;
    std::chrono::seconds max_idle_time_;
    size_t max_thr_size_;
    size_t min_thr_size_;
    std::atomic<int> cur_thr_size_;
    std::atomic<int> idle_thr_size_;
    std::atomic<bool> is_pool_running_;
    // std::once_flag flg_called_;

private:
    // 构造函数私有化
    CachedThreadPool()
        : task_que_(MAX_TASKQUEUE_SIZE),
          max_idle_time_(MAX_THREAD_IDLE_TIME),
          max_thr_size_(MAX_THREAD_SIZE),
          min_thr_size_(MIN_THREAD_SIZE),
          cur_thr_size_(0),
          idle_thr_size_(0),
          is_pool_running_(false)
    {
        is_pool_running_ = true;
        std::lock_guard<std::mutex> locker(mtx_);
        for (int i = 0; i < min_thr_size_; ++i)
        {
            auto pu_thr = std::make_unique<std::thread>(&CachedThreadPool::workthread, this);
            std::thread::id tid = pu_thr->get_id();
            map_tid_puThr_.emplace(tid, std::move(pu_thr));
            ++idle_thr_size_;
            ++cur_thr_size_;
        }
    }
    ~CachedThreadPool()
    {
        // std::call_once(flg_called_, [this]{ stop(); });
        task_que_.stop_r();
        is_pool_running_ = false;
        std::lock_guard<std::mutex> locker(mtx_);
        for (auto &x : map_tid_puThr_)
        {
            if (x.second && x.second->joinable())
            {
                x.second->detach();
                --cur_thr_size_;
            }
        }
        map_tid_puThr_.clear();
    }
    CachedThreadPool(const CachedThreadPool &) = delete;
    CachedThreadPool &operator=(const CachedThreadPool &) = delete;
    void workthread()
    {
        auto tid = std::this_thread::get_id();
        auto begin = std::chrono::high_resolution_clock::now();
        while (is_pool_running_)
        {
            if (task_que_.empty_r())
            {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::high_resolution_clock::now() - begin);
                std::lock_guard<std::mutex> locker(mtx_);
                if (duration >= max_idle_time_ && cur_thr_size_ > min_thr_size_)
                {
                    map_tid_puThr_.find(tid)->second->detach();
                    map_tid_puThr_.erase(tid);
                    --cur_thr_size_;
                    --idle_thr_size_;
                    std::clog << "idle thread " << tid << " end\n";
                    std::clog << "remaining idle thread size: " << idle_thr_size_ << "\n";
                    std::clog << "current thread size: " << cur_thr_size_ << "\n";
                    return;
                }
            }
            Task task;
            if (task_que_.take_r(task))
            {
                --idle_thr_size_;
                task();
                ++idle_thr_size_;
                begin = std::chrono::high_resolution_clock::now();
            }
        }
    }
    void addNewThread()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        if (idle_thr_size_ <= 0 && cur_thr_size_ < max_thr_size_)
        {
            auto pu_thr = std::make_unique<std::thread>(&CachedThreadPool::workthread, this);
            std::thread::id tid = pu_thr->get_id();
            map_tid_puThr_.emplace(tid, std::move(pu_thr));
            ++idle_thr_size_;
            ++cur_thr_size_;
        }
    }

public:
    static CachedThreadPool &_getInstance()
    {
        static CachedThreadPool instance;
        return instance;
    }
    template <class Func, class... Args>
    auto submit(Func &&task, Args &&...args)
    {
        using RtTy = decltype(task(args...));
        auto ps_promiseTask = std::make_shared<std::packaged_task<RtTy()>>(
            std::bind(std::forward<Func>(task), std::forward<Args>(args)...));
        std::future<RtTy> rt = ps_promiseTask->get_future();
        if (task_que_.put_r([ps_promiseTask]
                            { (*ps_promiseTask)(); }) != 0)
        {
            std::clog << "cannot put task into task queue\n";
            (*ps_promiseTask)();
        }
        addNewThread();
        return rt;
    }
    template <class Func>
    auto submit(Func &&task)
    {
        using RtTy = decltype(task());
        auto ps_promiseTask = std::make_shared<std::packaged_task<RtTy()>>(
            std::forward<Func>(task));
        std::future<RtTy> rt = ps_promiseTask->get_future();
        if (task_que_.put_r([ps_promiseTask]
                            { (*ps_promiseTask)(); }) != 0)
        {
            std::clog << "cannot put task into task queue\n";
            (*ps_promiseTask)();
        }
        addNewThread();
        return rt;
    }
    template <class Func, class... Args>
    void excute(Func &&task, Args &&...args)
    {
        auto ps_task = std::make_shared<std::function<void()>>(
            std::bind(std::forward<Func>(task), std::forward<Args>(args)...));
        if (task_que_.put_r([ps_task]
                            { (*ps_task)(); }) != 0)
        {
            std::clog << "cannot put task into task queue\n";
            (*ps_task)();
        }
        addNewThread();
    }
    void excute(Task &&task)
    {
        if (task_que_.put_r(std::forward<Task>(task)) != 0)
        {
            std::clog << "cannot put task into task queue\n";
            task();
        }
        addNewThread();
    }
    void excute(const Task &task)
    {
        if (task_que_.put_r(task) != 0)
        {
            std::clog << "cannot put task into task queue\n";
            task();
        }
        addNewThread();
    }
};

#endif
