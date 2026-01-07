#pragma once
#include <cstdint>

struct Count
{
    //Cuckoo Hash
    long long hashCount = 0;
    long long compareCount = 0;
    long long kickCount = 0;
    long long rehashCount = 0;

    //B-tree
    long long btreeNodeAccess = 0;
    long long btreeKeyCompare = 0;
    long long btreeSplitCount = 0;
    long long btreeDelete = 0;
    long long btreeMerge = 0;
    long long btreeBorrow = 0;

    void reset()
    {
        //reset toàn bộ counter
        *this = Count();
    }
};

extern Count gCount;
