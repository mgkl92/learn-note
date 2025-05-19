#include "../include/ThreadCache.hpp"
#include "../include/CentralCache.hpp"
#include <cstddef>

namespace mgkl {
    void * ThreadCache::allocate(size_t size) {
        // 至少分配一个对齐字节大小
        if (size == 0) {
            size = ALIGNMENT;
        }

        // 使用系统调处理大块内存（> 256KB）申请
        if (size > MAX_BYTES) {
            return malloc(size);
        }

        // 计算待分配内存在自由链表中的索引
        size_t index = SizeClass::getIndex(size);

        --freeListSize_[index];

        /*
         * 检查本地线程的自由链表
         * 如果自由链表不为空，则该链表中存在可以直接分配的内存块
         */
        if (void * p = freeList_[index]) {
            /**
             * ！适应平台的指针方案（需要保证内存对象的大小至少等于目标系统的地址大小）
             * 使用内存对象的前 4/8 个字节作为地址指向下一个县相邻的内存块
             * 即将内存对象强转为 void **, 然后在对二级指针类型解引用
             */
            freeList_[index] = *reinterpret_cast<void **>(p);
            return p;
        }

        return fetchFromCentralCache(index);
    }

    void ThreadCache::deallocate(void * p, size_t size) {
        // 使用系统调用释放申请的大块内存
        if (size > MAX_BYTES) {
            free(p);
            return;
        }

        // 计算待释放内存在自由链表中的索引
        size_t index = SizeClass::getIndex(size);

        // 以头插法将释放的内存块放到自由链表中
        *reinterpret_cast<void **>(p) = freeList_[index];
        freeList_[index] = p;
        ++freeListSize_[index];

        // freeListSize_ 用于监控自由链表中内存块的数目
        // 当自由链表的长度超过一定阈值时，部分内存对象返回给 CentralCache
        if (shouldReturnToCentralCache(index)) {
            returnToCentralCache(freeList_[index], size);
        }
    }

    bool ThreadCache::shouldReturnToCentralCache(size_t index) {
        size_t threshold = 64;

        return (freeListSize_[index] > threshold);
    }

    void * ThreadCache::fetchFromCentralCache(size_t index) {
        // 从中心缓存批量获取内存块
        void * start = CentralCache::getInstance().fetchRange(index);
        if (!start) {
            return nullptr;
        }

        // 取单个内存块作为返回结果，剩余内存块插入到自由链表
        void * res = start;
        freeList_[index] = *reinterpret_cast<void **>(start);

        size_t batchNum = 0;
        void * cur = start;

        while (cur != nullptr) {
            ++batchNum;
            cur = *reinterpret_cast<void **>(cur);
        }

        freeListSize_[index] += batchNum;

        return res;
    }

    void ThreadCache::returnToCentralCache(void * start, size_t size) {
        size_t index = SizeClass::getIndex(size);
        size_t alingedSize = SizeClass::ceil(size);

        // 获取当前自由列表中内存对象的数目
        size_t batchNum = freeListSize_[index];

        // 如果当前线程缓存仅有一块内存，直接返回
        if (batchNum <= 1) {
            return;
        }

        // 保留 1/4 内存对象于线程缓存中
        size_t keepNum = std::max(batchNum / 4, size_t(1));
        size_t returnNum = batchNum - keepNum;

        char * cur = static_cast<char *>(start);
        char * splitNode = cur;
        for (size_t i = 0; i < keepNum; ++i) {
            splitNode = reinterpret_cast<char *>(*reinterpret_cast<void **>(splitNode));
            if (splitNode == nullptr) {
                returnNum = batchNum - (i + 1);
                break;
            }
        }

        if (splitNode != nullptr) {
            // 将需要归还的内存对象从链表中取下
            void * nextNode = *reinterpret_cast<void **>(splitNode);
            *reinterpret_cast<void **>(splitNode) = nullptr;

            freeListSize_[index] = keepNum;

            if (returnNum > 0 && nextNode != nullptr) {
                CentralCache::getInstance().returnRange(nextNode, returnNum * alingedSize, index);
            }
        }
    }
} // namespace mgkl