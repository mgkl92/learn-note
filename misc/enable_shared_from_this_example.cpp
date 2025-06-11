/**
 * Learnong codes from https://en.cppreference.com/w/cpp/memory/enable_shared_from_this.html
 * the explaination of `enable_shared_from_this` from https://cloud.tencent.com/developer/article/2362395
 */
#include <iostream>
#include <memory>

class Good : public std::enable_shared_from_this<Good> {
public:
    std::shared_ptr<Good> getptr() {
        return shared_from_this();
    }
};

class Best : public std::enable_shared_from_this<Best> {
private:
    struct Private {
        explicit Private() = default;
    };

public:
    Best(Private) {
    }

    // make sure all the objects will be managed by `shared_ptr
    static std::shared_ptr<Best> create() {
        return std::make_shared<Best>(Private());
    }

    std::shared_ptr<Best> getptr() {
        return shared_from_this();
    }
};

struct Bad {
    std::shared_ptr<Bad> getptr() {
        return std::shared_ptr<Bad>(this);
    }

    ~Bad() {
        std::cout << "Bad::~Bad() called" << std::endl;
    }
};

void testGood() {
    std::shared_ptr<Good> good0 = std::make_shared<Good>();
    std::shared_ptr<Good> good1 = good0->getptr();

    std::cout << "good.use_count() = " << good1.use_count() << std::endl;
}

void misuseGood() {
    // Undefined-Behavior
    // `shared_from_this()` requires that the object refered by `this` should under the management of `shared_ptr`
    try {
        Good not_so_good;
        std::shared_ptr<Good> good1 = not_so_good.getptr();
    } catch (std::bad_weak_ptr &e) {
        std::cout << e.what() << std::endl;
    }
}

void testBest() {
    std::shared_ptr<Best> best0 = Best::create();
    std::shared_ptr<Best> best1 = best0->getptr();

    std::cout << "best1.use_count() = " << best1.use_count() << std::endl;
}

void testBad() {
    std::shared_ptr<Bad> bad0 = std::make_shared<Bad>();
    std::shared_ptr<Bad> bad1 = bad0->getptr();
    std::cout << "bad1.use_count() = " << bad1.use_count() << std::endl;

    // Undefined-Behavior: Double-Delete of Bad
}

int main(int argc, char const *argv[]) {
    /**
     * good.use_count() = 2
     * bad_weak_ptr
     * best1.use_count() = 2
     * bad1.use_count() = 1
     * Bad::~Bad() called
     * double free or corruption (out)
     * Aborted (core dumped)
     */

    testGood();

    misuseGood();

    testBest();

    testBad();
    return 0;
}
