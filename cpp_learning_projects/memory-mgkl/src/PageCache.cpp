#include "../include/PageCache.hpp"

#include <sys/mman.h>
#include <cstring>

namespace mgkl {
    void * PageCache::allocateSpan(size_t numPages) {
        std::lock_guard<std::mutex> lock(mutex_);

        // 从空闲链表中查找第一个页数大于等于 numPages 的 Span
        auto it = freeSpans_.lower_bound(numPages);
        if (it != freeSpans_.end()) {
            Span * span = it->second;

            // 将 Span 从空闲链表中移除
            if (span->next) {
                freeSpans_[it->first] = span->next;
            } else {
                freeSpans_.erase(it);
            }

            // 如果 Span 包含页数大于所需页数，对齐分割
            if (span->numPages > numPages) {
                // 分理出多余内存页
                Span * newSpan = new Span;
                newSpan->pageAddr = static_cast<char *>(span->pageAddr) + numPages * PAGE_SIZE;
                newSpan->numPages = span->numPages - numPages;
                newSpan->next = nullptr;

                // 如果不存在页数为 newSpan->numPages 的空闲列表，则自动创建
                auto & list = freeSpans_[newSpan->numPages];
                newSpan->next = list;
                list = newSpan;

                span->numPages = numPages;
            }

            spanMap_[span->pageAddr] = span;
            return span->pageAddr;
        }

        // 如果空闲链表中不存在这样的 Span，则使用系统调用进行分配
        void * memory = systemAlloc(numPages);
        if (!memory) {
            return nullptr;
        }

        Span * span = new Span;
        span->pageAddr = memory;
        span->numPages = numPages;
        span->next = nullptr;

        spanMap_[memory] = span;
        return memory;
    }

    void PageCache::deallocateSpan(void * p, size_t numPages) {
        std::lock_guard<std::mutex> lock(mutex_);

        // 如果 p 指向地址为非页缓存管理的地址，直接返回
        auto it = spanMap_.find(p);
        if (it == spanMap_.end()) {
            return;
        }

        Span * span = it->second;

        // 尝试合并相邻的 Span
        // 应注意 spanMap_ 不仅记录已分配的 span，而且记录已经释放到空闲列表中的 span
        // 如果存在两个内存地址相邻的 span 则将其合并
        void * nextAddr = static_cast<char *>(p) + numPages * PAGE_SIZE;
        auto nextIt = spanMap_.find(nextAddr);
        if (nextIt != spanMap_.end()) {
            Span * nextSpan = nextIt->second;

            bool found = false;
            auto & nextList = freeSpans_[nextSpan->numPages];

            // 考虑空闲链表仅包含一个元素的情况
            if (nextList == nextSpan) {
                nextList = nextSpan->next;
                found = true;
            } else if (nextList) {
                // 循环查找该元素
                Span * prev = nextList;
                while (prev->next) {
                    if (prev->next == nextSpan) {
                        prev->next = nextSpan->next;
                        found = true;
                        break;
                    }
                    prev = prev->next;
                }
            }

            if (found) {
                span->numPages += nextSpan->numPages;
                spanMap_.erase(nextAddr);
                delete nextSpan;
            }
        }

        auto & list = freeSpans_[span->numPages];
        span->next = list;
        list = span;
    }

    void * PageCache::systemAlloc(size_t numPages) {
        size_t size = numPages * PAGE_SIZE;

        void * p = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED) {
            return nullptr;
        }

        memset(p, 0, size);
        return p;
    }

} // namespace mgkl