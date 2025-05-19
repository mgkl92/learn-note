#pragma once

#include "Common.hpp"

namespace mgkl {
    class ThreadCache {
    public:
        // 单例模式
        static ThreadCache * getInstance() {
            static thread_local ThreadCache instance;
            return &instance;
        }

        void * allocate(size_t size);

        void deallocate(void * p, size_t size);

    private:
        ThreadCache() {
            freeList_.fill(nullptr);
            freeListSize_.fill(0);
        }

        void * fetchFromCentralCache(size_t index);

        void returnToCentralCache(void * start, size_t size);

        bool shouldReturnToCentralCache(size_t index);

    private:
        std::array<void *, FREE_LIST_SIZE> freeList_;
        std::array<size_t, FREE_LIST_SIZE> freeListSize_;
    };
} // namespace mgkl