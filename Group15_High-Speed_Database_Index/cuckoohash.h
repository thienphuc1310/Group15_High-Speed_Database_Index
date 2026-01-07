#include <cstddef>
#include <vector>
using namespace std;

class CuckooHash
{
public:
    explicit CuckooHash(size_t n);

    bool insert(int key);
    bool find(int key) const;
    bool erase(int key);
    size_t size() const;

private:
    static constexpr int EMPTY = -1;
    static constexpr int MAX_KICKS = 64;

    size_t cap;
    size_t sz;
    unsigned long long seed;

    // 2 bảng chỉ lưu KEY
    vector<int> t1, t2;

    void init(size_t n);
    size_t p1(int key) const;
    size_t p2(int key) const;
    bool insertnoibo(int key);
    void rehash();
};
