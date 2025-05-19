#pragma once

#include "Common.hpp"
#include <atomic>
#include <mutex>

namespace mgkl {
    class CentralCache {
    public:
        static CentralCache & getInstance() {
            static CentralCache instance;
            return instance;
        }

        void * fetchRange(size_t index);
        void returnRange(void * start, size_t size, size_t bytes);

    private:
        CentralCache() {
            for (size_t i = 0; i < FREE_LIST_SIZE; ++i) {
                centralFreeList_[i].store(nullptr, std::memory_order_relaxed);
                locks_[i].clear();
            }
        }

        void * fetchFromPageCache(size_t size);

    private:
        std::array<std::atomic<void *>, FREE_LIST_SIZE> centralFreeList_;

        std::array<std::atomic_flag, FREE_LIST_SIZE> locks_;
    };
} // namespace mgkl