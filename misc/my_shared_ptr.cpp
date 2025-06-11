/**
 * References
 * [1] https://zhuanlan.zhihu.com/p/384894475
 * [2] https://www.cnblogs.com/mochacoco/p/18522125
 */
#include <atomic>
#include <mutex>

namespace mgkl {
// no thread safety
template <class T>
class SharedPtr {
private:
    T *m_ptr;

    unsigned int *m_ref_count;

    void swap(SharedPtr &other) {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_ref_count, other.m_ref_count);
    }

public:
    SharedPtr() :
        m_ptr(nullptr), m_ref_count(nullptr) {
    }

    SharedPtr(T *ptr) :
        m_ptr(ptr), m_ref_count(new unsigned int(1)) {
    }

    SharedPtr(const SharedPtr &other) {
        m_ptr = other.m_ptr;
        m_ref_count = other.m_ref_count;

        if (m_ref_count) {
            ++(*m_ref_count);
        }
    }

    SharedPtr &operator=(const SharedPtr &rhs) {
        if (this != &rhs) {
            if (m_ref_count) {
                --(*m_ref_count);
                if (!(*m_ref_count)) {
                    delete m_ptr;
                    delete m_ref_count;
                }
            }

            m_ptr = rhs.m_ptr;
            m_ref_count = rhs.m_ref_count;

            if (m_ref_count) {
                ++(*m_ref_count);
            }
        }

        return *this;
    }

    SharedPtr(SharedPtr &&other) :
        m_ptr(nullptr), m_ref_count(nullptr) {
        other.swap(*this);
    }

    SharedPtr &operator=(SharedPtr &&rhs) {
        if (this != &rhs) {
            SharedPtr(std::move(rhs)).swap(*this);
        }
        return *this;
    }
};

// thread safety
template <class T>
class SafeSharedPtr {
private:
    T *m_ptr;
    std::atomic<unsigned int> *m_ref_count;
    std::mutex *mtx;

    void release() {
        bool shouldDelete = false;
        {
            std::lock_guard<std::mutex> lock(*mtx);
            if (m_ref_count && --(*m_ref_count) == 0) {
                delete m_ptr;
                delete m_ref_count;

                shouldDelete = true;
            }
        }

        if (shouldDelete) {
            delete mtx;
        }
    }

public:
    SafeSharedPtr() :
        m_ptr(nullptr), m_ref_count(new std::atomic<unsigned int>(0)), mtx(new std::mutex) {
    }

    explicit SafeSharedPtr(T *other) :
        m_ptr(other->m_ptr), m_ref_count(new atomic<unsigned int>(1)), mtx(new std::mutex) {
    }

    SafeSharedPtr(const SharedPtr &other) {
        std::lock_guard<std::mutex> lock(*other.mtx);

        m_ptr = other.m_ptr;
        m_ref_count = other.m_ref_count;
        mtx = other.mtx;
        ++(*m_ref_count);
    }

    SafeSharedPtr &operater = (const SafeSharedPtr &rhs) {
        if (this != &rhs) {
            release();

            std::lock_guard<std::mutex> lock(*rhs.mtx);

            m_ptr = rhs.m_ptr;
            m_ref_count = rhs.m_ref_count;
            mtx = rhs.mtx;
            ++(*m_ref_count);
        }

        return *this;
    }

    SafeSharedPtr(SharedPtr &&other) {
        std::lock_guard<std::mutex> lock(*other.mtx);

        m_ptr = other.m_ptr;
        m_ref_count = other.m_ref_count;
        mtx = other.mtx;

        other.m_ptr = nullptr;
        other.m_ref_count = nullptr;
        other.mtx = nullptr;
    }

    SafeSharedPtr &operator=(SharedPtr &&rhs) {
        if (this != &rhs) {
            release();

            std::lock_guard<std::mutex> lock(*rhs);

            m_ptr = rhs.m_ptr;
            m_ref_count = rhs.m_ref_count;
            mtx = rhs.mtx;

            rhs.m_ptr = nullptr;
            rhs.m_ref_count = nullptr;
            rhs.mtx = nullptr;
        }

        return *this;
    }

    int use_count() const {
        return m_ref_count ? m_ref_count.load() : 0;
    }

    T *get() const {
        std::lock_guard<std::mutex> lock(*mtx);
        return m_ptr;
    }

    T &operater *() const {
        return *get();
    }

    T*operater->() const {
        return get();
    }

    ~SafeSharedPtr() {
        release();
    }


};
} // namespace mgkl