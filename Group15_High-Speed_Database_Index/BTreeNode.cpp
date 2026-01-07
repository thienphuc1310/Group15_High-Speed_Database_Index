#include "BTreeNode.h"
#include "count.h"


BTreeNode::BTreeNode(int _t, bool _leaf)
{
    t = _t;
    leaf = _leaf;
}


BTreeNode* BTreeNode::search(int k)
{
    gCount.btreeNodeAccess++;

    int i = 0;
    while (i < (int)keys.size() && k > keys[i])
    {
        gCount.btreeKeyCompare++;
        i++;
    }

    if (i < (int)keys.size())
    {
        gCount.btreeKeyCompare++;
        if (keys[i] == k)
            return this;
    }

    if (leaf) return nullptr;
    return child[i]->search(k);
}


void BTreeNode::splitChild(int i, BTreeNode* y)
{
    gCount.btreeSplitCount++;

    BTreeNode* z = new BTreeNode(y->t, y->leaf);

    int mid = y->keys[t - 1];

    for (int j = 0; j < t - 1; j++)
        z->keys.push_back(y->keys[j + t]);

    if (!y->leaf)
        for (int j = 0; j < t; j++)
            z->child.push_back(y->child[j + t]);

    y->keys.resize(t - 1);
    if (!y->leaf) y->child.resize(t);

    child.insert(child.begin() + i + 1, z);
    keys.insert(keys.begin() + i, mid);
}


void BTreeNode::insertNonFull(int k)
{
    int i = (int)keys.size() - 1;

    if (leaf)
    {
        keys.push_back(k);
        while (i >= 0 && keys[i] > k)
        {
            gCount.btreeKeyCompare++;
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
    }
    else
    {
        while (i >= 0 && keys[i] > k)
        {
            gCount.btreeKeyCompare++;
            i--;
        }

        if (child[i + 1]->keys.size() == 2 * t - 1)
        {
            splitChild(i + 1, child[i + 1]);
            if (keys[i + 1] < k) i++;
        }
        child[i + 1]->insertNonFull(k);
    }
}


void BTreeNode::rangeQuery(int L, int R, vector<int>& out)
{
    gCount.btreeNodeAccess++;

    int i = 0;
    for (; i < (int)keys.size(); i++)
    {
        if (!leaf) child[i]->rangeQuery(L, R, out);

        if (keys[i] >= L && keys[i] <= R)
            out.push_back(keys[i]);

        if (keys[i] > R) return;
    }

    if (!leaf) child[i]->rangeQuery(L, R, out);
}


void BTreeNode::collectFirstK(int K, vector<int>& out)
{
    if ((int)out.size() >= K) return;

    gCount.btreeNodeAccess++;

    int i = 0;
    for (; i < (int)keys.size(); i++)
    {
        if (!leaf) child[i]->collectFirstK(K, out);
        if ((int)out.size() >= K) return;

        out.push_back(keys[i]);
        if ((int)out.size() >= K) return;
    }

    if (!leaf) child[i]->collectFirstK(K, out);
}


int BTreeNode::findKey(int k)
{
    int idx = 0;
    while (idx < (int)keys.size() && keys[idx] < k)
        idx++;
    return idx;
}

void BTreeNode::remove(int k)
{
    gCount.btreeDelete++;

    int idx = findKey(k);

    if (idx < (int)keys.size() && keys[idx] == k)
    {
        if (leaf) removeFromLeaf(idx);
        else removeFromNonLeaf(idx);
    }
    else
    {
        if (leaf) return;

        bool flag = (idx == (int)keys.size());

        if (child[idx]->keys.size() < t)
            fill(idx);

        if (flag && idx > (int)keys.size())
            child[idx - 1]->remove(k);
        else
            child[idx]->remove(k);
    }
}

void BTreeNode::removeFromLeaf(int idx)
{
    keys.erase(keys.begin() + idx);
}

void BTreeNode::removeFromNonLeaf(int idx)
{
    int k = keys[idx];

    if (child[idx]->keys.size() >= t)
    {
        int pred = getPred(idx);
        keys[idx] = pred;
        child[idx]->remove(pred);
    }
    else if (child[idx + 1]->keys.size() >= t)
    {
        int succ = getSucc(idx);
        keys[idx] = succ;
        child[idx + 1]->remove(succ);
    }
    else
    {
        merge(idx);
        child[idx]->remove(k);
    }
}

int BTreeNode::getPred(int idx)
{
    BTreeNode* cur = child[idx];
    while (!cur->leaf)
        cur = cur->child.back();
    return cur->keys.back();
}

int BTreeNode::getSucc(int idx)
{
    BTreeNode* cur = child[idx + 1];
    while (!cur->leaf)
        cur = cur->child[0];
    return cur->keys[0];
}


void BTreeNode::fill(int idx)
{
    if (idx != 0 && child[idx - 1]->keys.size() >= t)
        borrowFromPrev(idx);
    else if (idx != (int)keys.size() && child[idx + 1]->keys.size() >= t)
        borrowFromNext(idx);
    else
    {
        if (idx != (int)keys.size())
            merge(idx);
        else
            merge(idx - 1);
    }
}

void BTreeNode::borrowFromPrev(int idx)
{
    gCount.btreeBorrow++;

    BTreeNode* c = child[idx];
    BTreeNode* s = child[idx - 1];

    c->keys.insert(c->keys.begin(), keys[idx - 1]);
    if (!c->leaf)
        c->child.insert(c->child.begin(), s->child.back());

    keys[idx - 1] = s->keys.back();

    s->keys.pop_back();
    if (!s->leaf) s->child.pop_back();
}

void BTreeNode::borrowFromNext(int idx)
{
    gCount.btreeBorrow++;

    BTreeNode* c = child[idx];
    BTreeNode* s = child[idx + 1];

    c->keys.push_back(keys[idx]);
    if (!c->leaf)
        c->child.push_back(s->child[0]);

    keys[idx] = s->keys[0];

    s->keys.erase(s->keys.begin());
    if (!s->leaf) s->child.erase(s->child.begin());
}

void BTreeNode::merge(int idx)
{
    gCount.btreeMerge++;

    BTreeNode* c = child[idx];
    BTreeNode* s = child[idx + 1];

    c->keys.push_back(keys[idx]);

    for (int v : s->keys)
        c->keys.push_back(v);

    if (!c->leaf)
        for (auto x : s->child)
            c->child.push_back(x);

    keys.erase(keys.begin() + idx);
    child.erase(child.begin() + idx + 1);

    delete s;
}
