/**
 * Reference:
 * https://github.com/CandyConfident/HighPerformanceConcurrentServer/blob/master/timer/timer.h
 */

#pragma once

#include <unordered_map>
#include <mutex>

template <typename K, typename V>
class hash_map {
public:
    void emplace(const K& key, const V& val) {
        std::lock_guard<std::mutex> lock(mtx_);
        hmap_[key] = val;
    }

    void emplace(const K& key, V&& val) {
        std::lock_guard<std::mutex> lock(mtx_);
        hmap_[key] = std::move(val);
    }

    void erase(const K& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (hmap_.find(key) != hmap_.end()) {
            hmap_.erase(key);
        }
    }

    bool get(const K& key, V& val) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (hmap_.find(key) != hmap_.end()) {
            val = hmap_[key];
            return true;
        }
        return false;
    }

    bool exist(const K& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        return (hmap_.find(key) != hmap_.end());
    }   

    size_t size() {
        std::lock_guard<std::mutex> lock(mtx_);
        return hmap_.size();
    }

private:
    std::unordered_map<K, V> hmap_;
    std::mutex mtx_;
};