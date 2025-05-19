#pragma once

#include "ThreadCache.hpp"

namespace mgkl {
    class LeveledMemoryPool {
    public:
        static void * allocate(size_t size) {
            return ThreadCache::getInstance()->allocate(size);
        }

        static void deallocate(void * p, size_t size) {
            ThreadCache::getInstance()->deallocate(p, size);
        }
    };
} // namespace mgkl