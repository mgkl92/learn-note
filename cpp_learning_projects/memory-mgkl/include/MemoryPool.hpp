#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <utility>

namespace mgkl {
#define MEMORY_POOL_NUM 64
#define SLOT_BASE_SIZE 8
#define MAX_SLOT_SIZE 512

    struct Slot {
        std::atomic<Slot *> next;
    };

    class MemoryPool {
    public:
        MemoryPool(size_t blockSize = 4096);
        ~MemoryPool();

        void init(size_t size);

        void * allocate();
        void deallocate(void *);

    private:
        void allocateNewBlock();

        size_t padPointer(char * p, size_t align);

        bool pushFreeList(Slot * slot);
        Slot * popFreeList();

    private:
        size_t blockSize_;
        size_t slotSize_;
        Slot * firstBlock_;
        Slot * curSlot_;
        std::atomic<Slot *> freeList_;
        Slot * lastSlot_;
        std::mutex blockMutex_;
    };

    class HashBucket {
    public:
        static void initMemoryPool();

        static MemoryPool & getMemoryPool(size_t index);

        static void * useMemory(size_t nbytes) {
            if (nbytes <= 0) {
                return nullptr;
            }

            if (nbytes > MAX_SLOT_SIZE) {
                return operator new(nbytes);
            }

            return getMemoryPool((nbytes + 7) / SLOT_BASE_SIZE - 1).allocate();
        }

        static void freeMemory(void * ptr, size_t size) {
            if (!ptr) {
                return;
            }

            if (size > MAX_SLOT_SIZE) {
                operator delete(ptr);
                return;
            }

            getMemoryPool((size + 7) / SLOT_BASE_SIZE - 1).deallocate(ptr);
        }

        template <typename T, typename... Args>
        friend T * newElement(Args &&... args);

        template <typename T>
        friend void deleteElement(T * p);
    };

    template <typename T, typename... Args>
    T * newElement(Args &&... args) {
        T * p = nullptr;

        if ((p = reinterpret_cast<T *>(HashBucket::useMemory(sizeof(T)))) != nullptr) {
            new (p) T(std::forward<Args>(args)...);
        }

        return p;
    }

    template <typename T>
    void deleteElement(T * p) {
        if (p) {
            p->~T();

            HashBucket::freeMemory(reinterpret_cast<void *>(p), sizeof(T));
        }
    }

} // namespace mgkl