#pragma once

#include "Common.hpp"
#include <map>
#include <mutex>

namespace mgkl {
    class PageCache {
    public:
        // 页大小 4KB
        const static size_t PAGE_SIZE = 4096;

        static PageCache & getInstance() {
            static PageCache instance;
            return instance;
        }

        void * allocateSpan(size_t npages);

        void deallocateSpan(void * p, size_t npages);

    private:
        PageCache() = default;

        void * systemAlloc(size_t npages);

    private:
        struct Span {
            void * pageAddr; // 页起始位置
            size_t numPages; // 页数
            Span * next;
        };

        std::map<size_t, Span *> freeSpans_;

        // 管理已分配页地址与 SPAN 之间的映射
        std::map<void *, Span *> spanMap_;
        std::mutex mutex_;
    };
} // namespace mgkl