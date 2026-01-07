#pragma once
#include "BTreeNode.h"

class BTree
{
public:
    BTreeNode* root = nullptr;
    int t;

    BTree(int _t) : t(_t), root(nullptr) {}

    void insert(int k);
    BTreeNode* search(int k);

    void rangeQuery(int L, int R, vector<int>& out);
    void collectFirstK(int K, vector<int>& out);

    void remove(int k);
    void removeRange(int L, int R);
};
