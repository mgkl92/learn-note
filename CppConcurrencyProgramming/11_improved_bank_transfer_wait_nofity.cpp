#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <string>
#include <set>

using namespace std;

class Account {
public:
    Account(string name, double money) :
        name_(name), money_(money) {
    }

    const string &getName() const {
        return name_;
    }

    const double getMoney() const {
        return money_;
    }

    void changeMoney(double amount) {
        unique_lock<mutex> lock(money_lock_);
        cond_.wait(lock, [this, amount] {
            return money_ + amount > 0;
        });

        money_ += amount;

        cond_.notify_all();
    }

    mutex *getLock() {
        return &money_lock_;
    }

private:
    string name_;
    double money_;
    mutex money_lock_;
    condition_variable cond_;
};

class Bank {
public:
    void addAccount(Account *account) {
        accounts_.insert(account);
    }

    void transferMoney(Account *alice, Account *bob, double amount) {
        // lock_guard<mutex> alice_lock(*alice->getLock());
        // lock_guard<mutex> bob_lock(*bob->getLock());

        // lock(*alice->getLock(), *bob->getLock());
        // lock_guard<mutex> alice_lock(*alice->getLock(), adopt_lock);
        // lock_guard<mutex> bob_lock(*bob->getLock(), adopt_lock);

        // if (amount > alice->getMoney()) {
        //     return false;
        // }

        // alice->changeMoney(-amount);
        // bob->changeMoney(amount);

        // return true;

        alice->changeMoney(-amount);
        bob->changeMoney(amount);
    }

    double total() const {
        double sum = 0;
        for (auto &account : accounts_) {
            sum += account->getMoney();
        }

        return sum;
    }

private:
    set<Account *> accounts_;
};

// mutex total_lock;
void randomTransfer(Bank *bank, Account *alice, Account *bob) {
    // while (true) {
    //     double amount = ((double)rand() / RAND_MAX) * 100;
    //     if (bank->transferMoney(alice, bob, amount)) {
    //         cout << "Transfer " << amount << " from " << alice->getName()
    //              << " to " << bob->getName()
    //              << ", Bank totalMoney: " << bank->total() << endl;
    //     } else {
    //         cout << "Transfer failed, "
    //              << alice->getName() << " has only $" << alice->getMoney() << ", but "
    //              << amount << " required" << endl;
    //     }
    // }

    // ？
    static mutex total_lock;

    while (true) {
        double amount = ((double)rand() / RAND_MAX) * 100;
        {
            lock_guard<mutex> lock(total_lock);
            cout << "Try to Transfer " << amount
                 << " from " << alice->getName() << "(" << alice->getMoney()
                 << ") to " << bob->getName() << "(" << bob->getMoney()
                 << "), Bank totalMoney: " << bank->total() << endl;
        }
        bank->transferMoney(alice, bob, amount);
    }
}

int main(int argc, char const *argv[]) {
    Account alice("alice", 100);
    Account bob("bob", 100);

    Bank bank;

    bank.addAccount(&alice);
    bank.addAccount(&bob);

    thread t1(randomTransfer, &bank, &alice, &bob);
    thread t2(randomTransfer, &bank, &bob, &alice);

    t1.join();
    t2.join();

    /**
     * Note: The deadlock may occurs in this program.
     */
    return 0;
}
