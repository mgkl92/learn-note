#include <queue>

using std::queue;

template <typename T>
class BTreeNode {
    T data;
    BTreeNode *next;
};

// 层序遍历
template <typename T>
void LayerOrder(BTreeNode<T> *root) {
    if (root) {
        queue<BTreeNode *> q;
        q.push(root);

        while (!q.empty()) {
            int size = q.size();
            for (size_t i = 0; i < size; ++i) {
                BTreeNode *temp = q.front();
                q.pop();

                std::cout << temp->data << " ";
                
                if (temp->left) {
                    q.push(temp->left);
                }
                if (temp->right) {
                    q.push(temp->right);
                }
            }
            std::endl;
        }
    }
}