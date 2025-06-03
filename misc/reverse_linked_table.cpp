#include <iostream>
struct Node {
    int val;
    Node *next;

    Node(int val, Node *next = nullptr) :
        val(val), next(next) {
    }
};

Node *reverse(Node *head);

int main(int argc, char const *argv[]) {
    Node *head = new Node(0), *p = head;
    for (size_t i = 1; i < 5; ++i) {
        p->next = new Node(i);
        p = p->next;
    }

    p = head;
    while (p) {
        std::cout << p->val << std::endl;
        p = p->next;
    }

    p = reverse(head);
    while (p) {
        std::cout << p->val << std::endl;
        p = p->next;
    }

    return 0;
}

Node *reverse(Node *head) {
    if (!head) {
        return head;
    }

    Node *res = nullptr, *p = head;

    // while (p) {
    //     Node *tmp = p->next;

    //     p->next = res;
    //     res = p;

    //     p = tmp;
    // }

    while (p) {
        Node *q = p;
        p = p->next;
        q->next = res;
        res = q;
    }

    return res;
}