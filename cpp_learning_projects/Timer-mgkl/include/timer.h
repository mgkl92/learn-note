/**
 * Reference:
 * https://github.com/CandyConfident/HighPerformanceConcurrentServer/blob/master/timer/timer.h
 */
#pragma once

#include "hash_map.h"
// #include "../threadpool/threadpool.h"

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <chrono>

class Timer {
public:
    struct TimerNode {
        std::chrono::time_point<std::chrono::high_resolution_clock> tn_tm_point;
        std::function<void()> tn_callback;
        // int repeated_id;
        int tn_id;
        int tn_repeated_num;
        bool tn_is_period;
        bool tn_is_repeated;
        std::chrono::milliseconds tn_period;
        // bool operator<(const TimerNode& b) const { return (tn_tm_point > b.tn_tm_point); }
    };

    enum class IdState {
        Runing = 0
    };

    Timer() : tm_queue_(compare) {
        tm_id_.store(0);
        tm_runing_.store(true);
    }

    ~Timer() {
        tm_runing_.store(false);

        if (tm_tick_thread_.joinable()) {
            tm_tick_thread_.join();
        }

        tm_cond_.notify_all();
    }

    void run() {
        tm_tick_thread_ = std::move(std::thread([this]() {
            run_local();
            }));
    }

    bool is_available() {
        // return tm_thread_pool_.idl_thread_cnt() >= 0;
        return false;
    }

    int size() {
        return tm_queue_.size();
    }

    // 添加周期性任务
    template<typename F, typename...Args>
    int run_after(int ms_time, bool is_period, F&& f, Args&&... args) {
        TimeNode s;

        s.tn_id = tm_id_.fetch_add(1);
        tm_id_state_map_.emplace(s.tn_id, IdState::Runing);
        s.tn_is_period = is_period;
        s.tn_period = std::chrono::milliseconds(ms_time);
        s.tn_tm_point = std::chrono::high_resolution_clock::now() + s.tn_period;
        s.tn_callback = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        std::lock_guard<std::mutex> lock(tm_mutex_);
        tm_queue_.push(s);
        tm_cond_.notify_all();

        return s.tn_id;
    }

    // 添加一次性任务
    // template<typename F, typename...Args>
    // int run_at(const std::chrono::time_point<std::chrono::high_resolution_clock>& tm_point, F&& f, Args&&... args) {
    //     TimerNode s;

    //     s.tn_id = tm_id_.fetch_add(1);
    //     tm_id_state_map_.emplace(s.tn_id, IdState::Runing);
    //     s.tn_is_period = false;
    //     s.tn_tm_point = tm_point;
    //     s.tn_callback = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    //     std::lock_guard<mutex> lock(tm_mutex_);
    //     tm_queue_.push(s);
    //     tm_cond_.notify_all();

    //     return s.tn_id;
    // }

    // 添加可重复任务（一次性任务为可重复任务的特殊情况）
    template<typename F, typename...Args>
    int run_repeated(int ms_time, int repeated_num = 1, F&& f, Args&&... args) {
        TimerNode s;

        s.tn_id = tm_id_.fetch_add(1);
        tm_id_state_map_.emplace(s.tn_id, IdState::Runing);
        s.tn_is_repeated = true;
        s.tn_repeated_num = repeated_num;
        s.tn_period = std::chrono::microseconds(ms_time);
        s.tn_tm_point = std::chrono::high_resolution_clock::now() + s.tn_period;
        s.tn_callback = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        std::lock_guard<mutex> lock(tm_mutex_);
        tm_queue_.push(s);
        tm_cond_.notify_all();

        return s.tn_id;
    }

    void cancel(int id) {
        // 线程安全
        // 但取消定时需要两次锁定互斥量，可能导致定时任务在第二次锁定互斥量前已经完成
        // 需注意：在定时任务完成时，并不会将该 id 从 tm_id_state_map_ 中移除
        // if (tm_id_state_map_.exist(id)) {
        //     tm_id_state_map_.erase(id);
        // }
        
        // 直接移除，erase 的实现逻辑包含存在性判断
        tm_id_state_map_.erase(id);
    }

private:
    void run_local() {
        while (tm_runing_.load())
        {
            std::unique_lock<std::mutex> lock(tm_mutex_);
            if (tm_queue_.empty()) {
                tm_cond_.wait(lock);
                continue;
            }

            TimerNode s = tm_queue_.top();
            auto diff = s.tn_tm_point - std::chrono::high_resolution_clock::now();
            // 检查是否需要继续定时等待
            if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() > 0) {
                tm_cond_.wait_for(lock, diff);
            }
            else {
                tm_queue_.pop();

                // 检查定时任务是否被取消
                if (!tm_id_state_map_.exist(s.tn_id)) {
                    continue;
                }

                // 判断任务类型
                if (s.tn_is_period) {
                    s.tn_tm_point = std::chrono::high_resolution_clock::now() + s.tn_period;
                    tm_queue_.push(s);
                }
                else if (s.tn_is_repeated && s.tn_repeated_num > 0) {
                    s.tn_tm_point = std::chrono::high_resolution_clock::now() + s.tn_period;
                    --s.tn_repeated_num;
                    tm_queue_.push(s);
                }
                lock.unlock();
                // tm_thread_pool.post_task(std::move(s.tn_callback));
            }
        }
    }

    std::function<bool(const TimerNode&, const TimerNode&)> compare = [](const TimerNode& lhs, const TimerNode& rhs) -> bool {
        return lhs.tn_tm_point > rhs.tn_tm_point;
        };

    // 使用优先队列处理定时任务
    std::priority_queue<TimerNode, std::vector<TimerNode>, decltype(compare)> tm_queue_;
    std::atomic<bool> tm_runing_;
    std::mutex tm_mutex_;
    std::condition_variable tm_cond_;
    std::thread tm_tick_thread_;

    // Threadpool tm_thread_pool_;
    std::atomic<int> tm_id_;
    // 记录定时任务的状态, 用于实现任务取消
    // 目前 enum IdState 仅包含一种状态
    // 在该定时器实现中，仅通过检查 tn_id 是否存在于 hash_map 中，来决定该任务是否已经被取消
    // 可考虑使用 hash_set
    hash_map<int, IdState> tm_id_state_map_;
};