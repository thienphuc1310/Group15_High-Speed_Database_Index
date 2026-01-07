#include "cuckoohash.h"
#include "count.h"

#include <cstdint> //uint64_t
#include <vector>
#include <algorithm>

using namespace std;


static uint64_t splitmix64(uint64_t x)
{
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

void CuckooHash::init(size_t n)
{
    cap = 1;
    while (cap < n) cap <<= 1;

    t1.assign(cap, EMPTY);
    t2.assign(cap, EMPTY);

    sz = 0;
}

size_t CuckooHash::p1(int key) const
{
    gCount.hashCount++;
    return splitmix64((uint64_t)(uint32_t)key ^ seed) & (cap - 1);
}

size_t CuckooHash::p2(int key) const
{
    gCount.hashCount++;
    return splitmix64( (uint64_t)(uint32_t)key ^ (seed * 0x9e3779b97f4a7c15ULL)) & (cap - 1);
}

//insert nội bộ trước khi rehash
bool CuckooHash::insertnoibo(int key)
{
    int curKey = key;
    bool inFirst = true;

    for (int step = 0; step < MAX_KICKS; step++)
    {
        gCount.kickCount++;

        if (inFirst)
        {
            size_t i = p1(curKey);
            if (t1[i] == EMPTY)
            {
                t1[i] = curKey;
                sz++;
                return true;
            }
            swap(curKey, t1[i]);
            inFirst = false;
        }
        else
        {
            size_t i = p2(curKey);
            if (t2[i] == EMPTY)
            {
                t2[i] = curKey;
                sz++;
                return true;
            }
            swap(curKey, t2[i]);
            inFirst = true;
        }
    }
    return false; // vượt quá lần kick, ngắt
}

//rehash
void CuckooHash::rehash()
{
    gCount.rehashCount++;

    seed = splitmix64(seed + 0xABCDEF);

    vector<int> keys;
    keys.reserve(sz);

    for (size_t i = 0; i < cap; i++)
        if (t1[i] != EMPTY)
            keys.push_back(t1[i]);

    for (size_t i = 0; i < cap; i++)
        if (t2[i] != EMPTY)
            keys.push_back(t2[i]);

    init(cap * 2);

    for (int k : keys)
        insertnoibo(k);
}

CuckooHash::CuckooHash(size_t n)
{
    seed = 1;
    init(n);
}

size_t CuckooHash::size() const
{
    return sz;
}

//insert key 
bool CuckooHash::insert(int key)
{
    if (find(key)) return true;

    //rehash sớm tránh cycle dẫn đến trường hợp xấu nhất
    if ((double)(sz + 1) / cap > 0.45)
        rehash();

    if (insertnoibo(key))
        return true;

    rehash();
    return insertnoibo(key);
}

//tìm key
bool CuckooHash::find(int key) const
{
    size_t i1 = p1(key);
    if (t1[i1] == key)
        return true;

    size_t i2 = p2(key);
    if (t2[i2] == key)
        return true;

    return false;
}

//xóa key
bool CuckooHash::erase(int key)
{
    size_t i1 = p1(key);
    if (t1[i1] == key)
    {
        t1[i1] = EMPTY;
        sz--;
        return true;
    }

    size_t i2 = p2(key);
    if (t2[i2] == key)
    {
        t2[i2] = EMPTY;
        sz--;
        return true;
    }

    return false;
}
