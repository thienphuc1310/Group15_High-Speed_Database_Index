#include "BTree.h"

//insert key
void BTree::insert(int k)
{
    if (root == nullptr)
    {
        root = new BTreeNode(t, true);
        root->keys.push_back(k);
        return;
    }

    if (root->keys.size() == 2 * t - 1)
    {
        BTreeNode* s = new BTreeNode(t, false);
        s->child.push_back(root);
        s->splitChild(0, root);

        int i = (s->keys[0] < k) ? 1 : 0;
        s->child[i]->insertNonFull(k);
        root = s;
    }
    else
    {
        root->insertNonFull(k);
    }
}

//tìm kiếm key
BTreeNode* BTree::search(int k)
{
    if (!root) return nullptr;
    return root->search(k);
}

void BTree::rangeQuery(int L, int R, std::vector<int>& out)
{
    if (root)
        root->rangeQuery(L, R, out);
}

void BTree::collectFirstK(int K, std::vector<int>& out)
{
    if (root)
        root->collectFirstK(K, out);
}

//xóa 1 key nào đó
void BTree::remove(int k)
{
    if (!root) return;

    root->remove(k);

    if (root->keys.size() == 0)
    {
        BTreeNode* tmp = root;

        if (root->leaf)
            root = nullptr;
        else
            root = root->child[0];

        delete tmp;
    }
}

//xóa key trong khoảng
void BTree::removeRange(int L, int R)
{
    std::vector<int> keys;
    rangeQuery(L, R, keys);

    for (int k : keys)
        remove(k);
}
