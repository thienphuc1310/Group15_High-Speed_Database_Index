#pragma once
#include <vector>
using namespace std;

class BTreeNode
{
public:
    int t;
    bool leaf;
    vector<int> keys;
    vector<BTreeNode*> child;

    BTreeNode(int _t, bool _leaf);

    BTreeNode* search(int k);
    void splitChild(int i, BTreeNode* y);
    void insertNonFull(int k);

    void rangeQuery(int L, int R, vector<int>& out);
    void collectFirstK(int K, vector<int>& out);

    void remove(int k);

private:
    int findKey(int k);

    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);

    int getPred(int idx);
    int getSucc(int idx);

    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);
};
