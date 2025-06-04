#include "../include/MemoryPool.hpp"
#include <atomic>

namespace mgkl {
    MemoryPool::MemoryPool(size_t blockSize):
        blockSize_(blockSize),
        slotSize_(0),
        firstBlock_(nullptr),
        curSlot_(nullptr),
        freeList_(nullptr),
        lastSlot_(nullptr) {}

    MemoryPool::~MemoryPool() {
        Slot * p = firstBlock_;
        while (p) {
            Slot * q = p->next;
            operator delete(reinterpret_cast<void *>(p));
            p = q;
        }
    }

    void MemoryPool::init(size_t slotSize) {
        assert(slotSize > 0);
        slotSize_ = slotSize;
        firstBlock_ = nullptr;
        curSlot_ = nullptr;
        freeList_ = nullptr;
        lastSlot_ = nullptr;
    }

    void * MemoryPool::allocate() {
        // 优先使用空闲队列中的内存
        Slot * slot = popFreeList();
        if (slot) {
            return slot;
        }

        Slot * t;
        {
            std::lock_guard<std::mutex> lock(blockMutex_);

            // 分配新的内存空间
            if (curSlot_ >= lastSlot_) {
                allocateNewBlock();
            }

            t = curSlot_;
             // Slot 结构体本身只存储一个指针变量，不储内存大小相关信息
             // 由 slotSize_ 决定当前内存池对象存储 Slot 实际的大小
            curSlot_ += slotSize_ / sizeof(Slot); 
        }

        return t;
    }

    void MemoryPool::deallocate(void * p) {
        if (!p) {
            return;
        }

        Slot * t = reinterpret_cast<Slot *>(p);
        pushFreeList(t);
    }

    void MemoryPool::allocateNewBlock() {
        // 申请新的内存块
        void * t = operator new(blockSize_);

        // 头插法
        // 将整个 block 视为一个 Slot
        (reinterpret_cast<Slot *>(t))->next = firstBlock_;
        firstBlock_ = reinterpret_cast<Slot *>(t);

        // 字节对齐
        char * body = reinterpret_cast<char *>(t) + sizeof(Slot *);
        size_t padding = padPointer(body, slotSize_);
        curSlot_ = reinterpret_cast<Slot *>(body + padding);

        lastSlot_ = reinterpret_cast<Slot *>(reinterpret_cast<size_t>(t) + blockSize_ - slotSize_ + 1);
        freeList_ = nullptr;
    }

    bool MemoryPool::pushFreeList(Slot * slot) {
        while (true) {
            Slot * oldHead = freeList_.load(std::memory_order_relaxed);
            slot->next.store(oldHead, std::memory_order_relaxed);

            if (freeList_.compare_exchange_weak(oldHead, slot, std::memory_order_release, std::memory_order_relaxed)) {
                return true;
            }
        }
    }

    size_t MemoryPool::padPointer(char * p, size_t align) {
        return (align - reinterpret_cast<size_t>(p)) % align;
    }

    Slot * MemoryPool::popFreeList() {
        while (true) {
            Slot * oldHead = freeList_.load(std::memory_order_relaxed);
            if (!oldHead) {
                return nullptr;
            }

            Slot * newHead = nullptr;
            try {
                newHead = oldHead->next.load(std::memory_order_relaxed);
            } catch (...) {
                continue;
            }

            if (freeList_.compare_exchange_weak(oldHead, newHead, std::memory_order_acquire, std::memory_order_relaxed)) {
                return oldHead;
            }
        }
    }

    void HashBucket::initMemoryPool() {
        for (size_t i = 0; i < MEMORY_POOL_NUM; ++i) {
            getMemoryPool(i).init((i + 1) * SLOT_BASE_SIZE);
        }
    }

    MemoryPool & HashBucket::getMemoryPool(size_t index) {
        static MemoryPool memoryPool[MEMORY_POOL_NUM];
        return memoryPool[index];
    }

} // namespace mgkl