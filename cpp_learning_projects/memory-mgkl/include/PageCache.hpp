#pragma once

#include "Common.hpp"
#include <map>
#include <mutex>

namespace mgkl {
    /**
     * 1. PageCache 在底层使用使用 Span 结构体对内存进行管理；
     * 2. 使用 freeSpans_ 来管理回收的内存空间；
     * 3. 使用 spanMap_ 来管理由程序向系统申请的内存空间。
     * 
     * 注：spanMap_ 中 Span 对象存在两种状态：
     *      1. 已分配；
     *      2. 已回收；
     * 
     * 注：PageCache 在向系统归还内存空间时，尝试合并相邻的 Span，这样的好处就是能够缓解内存空间分配的碎片化问题。
     */
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