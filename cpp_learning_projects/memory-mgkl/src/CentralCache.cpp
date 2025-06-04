#include "../include/CentralCache.hpp"
#include "../include/PageCache.hpp"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <thread>

namespace mgkl {
    // 每次向 PageCache 申请缓存页数
    static const size_t SPAN_PAGES = 8;

    /**
     * 
     */
    void * CentralCache::fetchRange(size_t index) {
        // 使用系统调用申请较大内存空间
        if (index >= FREE_LIST_SIZE) {
            return nullptr;
        }

        // 自旋锁
        // 保证 ThreadCache 对 CentralCache 相应自由链表的互斥访问
        // 但不影响其他 ThreadCache 对 CentralCache 其他自由链表的访问
        while (locks_[index].test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        void * res = nullptr;
        try {
            // 尝试从中心缓存获取内存块
            res = centralFreeList_[index].load(std::memory_order_relaxed);

            if (!res) {
                // 中心缓存为空，从页缓存获取新的内存块
                size_t size = (index + 1) * ALIGNMENT;
                res = fetchFromPageCache(size);

                if (!res) {
                    locks_[index].clear(std::memory_order_release);
                    return nullptr;
                }

                // 将获取内存块切分成小块内存
                char * start = static_cast<char *>(res);
                size_t blockNum = (SPAN_PAGES * PageCache::PAGE_SIZE) / size;

                // void ** 的用法参考 ThreadCache.cpp 中的注释
                if (blockNum > 1) {
                    for (size_t i = 1; i < blockNum; ++i) {
                        // 对地址进行偏移
                        void * cur = start + (i - 1) * size;
                        void * next = start + i * size;
                        // 等价于 cur->next
                        *reinterpret_cast<void **>(cur) = next;
                    }
                    *reinterpret_cast<void **>(start + (blockNum - 1) * size) = nullptr;

                    void * next = *reinterpret_cast<void **>(res);
                    *reinterpret_cast<void **>(res) = nullptr;

                    centralFreeList_[index].store(next, std::memory_order_acquire);
                }
            } else {
                // 直接从中心缓存自由链表中申请内存对象
                void * next = *reinterpret_cast<void **>(res);
                *reinterpret_cast<void **>(res) = nullptr;

                centralFreeList_[index].store(next, std::memory_order_acquire);
            }
        } catch (...) {
            // 确保异常发生锁仍能够被正常释放
            locks_[index].clear(std::memory_order_release);
            throw;
        }

        // 释放锁
        locks_[index].clear(std::memory_order_release);

        return res;
    }

    void CentralCache::returnRange(void * start, size_t size, size_t index) {
        // 释放由系统调用申请的内存块
        if (!start || index >= FREE_LIST_SIZE) {
            return;
        }

        while (locks_[index].test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        try {
            void * end = start;
            size_t count = 1;

            // 将指定数量的内存对象返还给中心缓存
            while (*reinterpret_cast<void **>(end) != nullptr && count < size) {
                end = *reinterpret_cast<void **>(end);
                ++count;
            }

            void * cur = centralFreeList_[index].load(std::memory_order_relaxed);
            *reinterpret_cast<void **>(end) = cur;
            centralFreeList_[index].store(start, std::memory_order_release);
        } catch (...) {
            locks_[index].clear(std::memory_order_release);
            throw;
        }

        locks_[index].clear(std::memory_order_release);
    }

    void * CentralCache::fetchFromPageCache(size_t size) {
        // 计算实际需要页数
        size_t numPages = (size + PageCache::PAGE_SIZE - 1) / PageCache::PAGE_SIZE;

        if (size <= SPAN_PAGES * PageCache::PAGE_SIZE) {
            // 小于等于 32KB 的请求，按固定页数分配
            return PageCache::getInstance().allocateSpan(SPAN_PAGES);
        } else {
            // 对于大于 32KB 的请求，按实际需求分配
            return PageCache::getInstance().allocateSpan(numPages);
        }
    }
} // namespace mgkl